#!/usr/bin/env python3
"""
Astree Python GUI - Optical Design Analysis Tool

A Python port of the Astree optical design application.
Provides a graphical interface for loading optical designs,
analyzing image quality, and visualizing results.
"""

import sys
import os
import math

# Import matplotlib later to avoid backend issues
plt = None
FigureCanvas = None
NavigationToolbar = None

def import_matplotlib():
    """Import matplotlib components when needed."""
    global plt, FigureCanvas, NavigationToolbar
    if plt is None:
        import matplotlib
        matplotlib.use('Qt5Agg')
        import matplotlib.pyplot as plt
        from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
        from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

try:
    from PyQt6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                                 QHBoxLayout, QLabel, QPushButton, QTextEdit,
                                 QFileDialog, QSplitter, QTableWidget, QTableWidgetItem,
                                 QHeaderView, QGroupBox, QProgressBar, QStatusBar,
                                 QMenuBar, QMenu, QMessageBox, QTabWidget)
    from PyQt6.QtCore import Qt, QTimer, pyqtSignal
    from PyQt6.QtGui import QAction, QIcon
    PYQT_VERSION = 6
except ImportError:
    try:
        from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                                     QHBoxLayout, QLabel, QPushButton, QTextEdit,
                                     QFileDialog, QSplitter, QTableWidget, QTableWidgetItem,
                                     QHeaderView, QGroupBox, QProgressBar, QStatusBar,
                                     QMenuBar, QMenu, QMessageBox, QTabWidget)
        from PyQt5.QtCore import Qt, QTimer, pyqtSignal
        from PyQt5.QtGui import QAction, QIcon
        PYQT_VERSION = 5
    except ImportError:
        print("Error: PyQt6 or PyQt5 is required for the GUI.")
        print("Install with: pip install PyQt6")
        sys.exit(1)

# Import our optical simulation modules
from optical_device import OpticalDevice
from device_io import load_device

class PlotCanvas(QWidget):
    """Matplotlib canvas for plotting optical analysis results."""

    def __init__(self, parent=None, width=8, height=6, dpi=100):
        super().__init__(parent)
        import_matplotlib()
        self.fig, self.axes = plt.subplots(figsize=(width, height), dpi=dpi)
        self.canvas = FigureCanvas(self.fig)
        layout = QVBoxLayout()
        layout.addWidget(self.canvas)
        self.setLayout(layout)

    def plot_spot_size_vs_angle(self, angles, spot_sizes, vignetting, title="Spot Size vs Field Angle"):
        """Plot spot size and vignetting vs field angle."""
        import_matplotlib()
        self.axes.clear()

        # Create twin axes for spot size and vignetting
        ax1 = self.axes
        ax2 = ax1.twinx()

        # Plot spot size
        line1 = ax1.plot(angles, spot_sizes, 'bo-', linewidth=2, markersize=6,
                        label='Spot Size')
        ax1.set_xlabel('Field Angle (degrees)')
        ax1.set_ylabel('Spot Size (μm)', color='b')
        ax1.tick_params(axis='y', labelcolor='b')

        # Plot vignetting
        line2 = ax2.plot(angles, vignetting, 'ro-', linewidth=2, markersize=6,
                        label='Vignetting')
        ax2.set_ylabel('Vignetting (%)', color='r')
        ax2.tick_params(axis='y', labelcolor='r')
        ax2.set_ylim(0, 105)

        # Add title and grid
        ax1.set_title(title)
        ax1.grid(True, alpha=0.3)

        # Combine legends
        lines = line1 + line2
        labels = [l.get_label() for l in lines]
        ax1.legend(lines, labels, loc='upper left')

        self.fig.tight_layout()
        self.canvas.draw()

class AstreeMainWindow(QMainWindow):
    """Main window for the Astree Python optical design application."""

    def __init__(self):
        super().__init__()
        self.optical_device = None
        self.current_file = ""
        self.init_ui()

    def init_ui(self):
        """Initialize the user interface."""
        self.setWindowTitle("Astree Python - Optical Design Analysis")
        self.setGeometry(100, 100, 1200, 800)

        # Create central widget and main layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QHBoxLayout(central_widget)

        # Create splitter for resizable panels
        splitter = QSplitter(Qt.Orientation.Horizontal)
        main_layout.addWidget(splitter)

        # Left panel - Design information and controls
        left_panel = self.create_left_panel()
        splitter.addWidget(left_panel)

        # Right panel - Results and plots
        right_panel = self.create_right_panel()
        splitter.addWidget(right_panel)

        # Set splitter proportions
        splitter.setSizes([400, 800])

        # Create menu bar
        self.create_menu_bar()

        # Create status bar
        self.status_bar = self.statusBar()
        self.status_bar.showMessage("Ready")

        # Initialize with empty state
        self.update_display()

    def create_left_panel(self):
        """Create the left panel with design information and controls."""
        panel = QWidget()
        layout = QVBoxLayout(panel)

        # File operations group
        file_group = QGroupBox("File Operations")
        file_layout = QVBoxLayout(file_group)

        self.load_button = QPushButton("Load Design (.astree)")
        self.load_button.clicked.connect(self.load_design)
        file_layout.addWidget(self.load_button)

        self.reload_button = QPushButton("Reload")
        self.reload_button.clicked.connect(self.reload_design)
        self.reload_button.setEnabled(False)
        file_layout.addWidget(self.reload_button)

        layout.addWidget(file_group)

        # Design information group
        info_group = QGroupBox("Design Information")
        info_layout = QVBoxLayout(info_group)

        self.design_info_text = QTextEdit()
        self.design_info_text.setMaximumHeight(200)
        self.design_info_text.setReadOnly(True)
        info_layout.addWidget(self.design_info_text)

        layout.addWidget(info_group)

        # Analysis controls group
        analysis_group = QGroupBox("Analysis")
        analysis_layout = QVBoxLayout(analysis_group)

        self.analyze_button = QPushButton("Analyze Image Quality")
        self.analyze_button.clicked.connect(self.analyze_image_quality)
        self.analyze_button.setEnabled(False)
        analysis_layout.addWidget(self.analyze_button)

        self.progress_bar = QProgressBar()
        self.progress_bar.setVisible(False)
        analysis_layout.addWidget(self.progress_bar)

        layout.addWidget(analysis_group)

        # Results summary
        results_group = QGroupBox("Results Summary")
        results_layout = QVBoxLayout(results_group)

        self.results_text = QTextEdit()
        self.results_text.setMaximumHeight(150)
        self.results_text.setReadOnly(True)
        results_layout.addWidget(self.results_text)

        layout.addWidget(results_group)

        return panel

    def create_right_panel(self):
        """Create the right panel with results and plots."""
        panel = QWidget()
        layout = QVBoxLayout(panel)

        # Tab widget for different views
        self.tab_widget = QTabWidget()

        # Image Quality tab
        iq_tab = QWidget()
        iq_layout = QVBoxLayout(iq_tab)

        self.iq_table = QTableWidget()
        self.iq_table.setColumnCount(4)
        self.iq_table.setHorizontalHeaderLabels(["Field Angle (°)", "Spot Size (μm)", "Spot/Airy", "Vignetting (%)"])
        self.iq_table.horizontalHeader().setStretchLastSection(True)
        iq_layout.addWidget(self.iq_table)

        self.tab_widget.addTab(iq_tab, "Image Quality")

        # Plot tab
        plot_tab = QWidget()
        plot_layout = QVBoxLayout(plot_tab)

        self.plot_canvas = PlotCanvas(plot_tab, width=8, height=6)
        plot_layout.addWidget(self.plot_canvas)

        # Add matplotlib navigation toolbar
        toolbar = NavigationToolbar(self.plot_canvas.canvas, plot_tab)
        plot_layout.addWidget(toolbar)

        self.tab_widget.addTab(plot_tab, "Spot Size Plot")

        layout.addWidget(self.tab_widget)

        return panel

    def create_menu_bar(self):
        """Create the menu bar."""
        menubar = self.menuBar()

        # File menu
        file_menu = menubar.addMenu('File')

        load_action = QAction('Load Design...', self)
        load_action.triggered.connect(self.load_design)
        file_menu.addAction(load_action)

        reload_action = QAction('Reload', self)
        reload_action.triggered.connect(self.reload_design)
        reload_action.setEnabled(False)
        self.reload_action = reload_action
        file_menu.addAction(reload_action)

        file_menu.addSeparator()

        exit_action = QAction('Exit', self)
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)

        # Help menu
        help_menu = menubar.addMenu('Help')

        about_action = QAction('About', self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)

    def load_design(self):
        """Load an optical design file."""
        file_dialog = QFileDialog()
        file_dialog.setNameFilter("Astree files (*.astree)")
        file_dialog.setDirectory("c:/dev/Astree/samples")

        if file_dialog.exec():
            filename = file_dialog.selectedFiles()[0]
            self.load_design_file(filename)

    def load_design_file(self, filename):
        """Load a design from file."""
        try:
            self.optical_device = load_device(filename)
            if self.optical_device:
                self.current_file = filename
                self.status_bar.showMessage(f"Loaded: {os.path.basename(filename)}")
                self.reload_action.setEnabled(True)
                self.analyze_button.setEnabled(True)
                self.update_display()
            else:
                QMessageBox.warning(self, "Load Error", "Failed to load design file.")
        except Exception as e:
            QMessageBox.critical(self, "Load Error", f"Error loading file: {str(e)}")

    def reload_design(self):
        """Reload the current design."""
        if self.current_file:
            self.load_design_file(self.current_file)

    def update_display(self):
        """Update the display with current design information."""
        if not self.optical_device:
            self.design_info_text.setPlainText("No design loaded.")
            self.results_text.setPlainText("")
            self.iq_table.setRowCount(0)
            return

        # Update design information
        info_text = f"File: {os.path.basename(self.current_file)}\n"
        info_text += f"Surfaces: {self.optical_device.nb_surface()}\n"
        info_text += f"FOV: ±{self.optical_device.half_field_of_view()}°\n"
        info_text += f"Field points: {self.optical_device.nb_intermediate_angles()}\n\n"

        info_text += "Surface Details:\n"
        for i in range(self.optical_device.nb_surface()):
            surf_type = self.optical_device.type(i)
            diameter = self.optical_device.get(i, OpticalDevice.DIAMETER)
            radius = self.optical_device.get(i, OpticalDevice.RADIUS_CURVATURE)
            thick = self.optical_device.get(i, OpticalDevice.THICK)
            info_text += f"  {i}: {surf_type}, D={diameter:.1f}mm, R={radius:.1f}mm, T={thick:.3f}mm\n"

        self.design_info_text.setPlainText(info_text)

    def analyze_image_quality(self):
        """Analyze the image quality of the current design."""
        if not self.optical_device:
            return

        self.status_bar.showMessage("Analyzing image quality...")
        self.progress_bar.setVisible(True)
        self.progress_bar.setRange(0, 0)  # Indeterminate progress

        # Perform analysis in a timer to keep GUI responsive
        QTimer.singleShot(100, self.perform_analysis)

    def perform_analysis(self):
        """Perform the actual image quality analysis."""
        try:
            iq = self.optical_device.get_image_quality()

            # Update results summary
            results_text = f"F-Number: {iq.dFNumber:.1f}\n"
            results_text += f"Airy disk: {iq.dAirySize:.1f} μm\n"
            results_text += f"Field points: {iq.nb_angles()}\n\n"

            # Update table
            self.iq_table.setRowCount(iq.nb_angles())
            angles = []
            spot_sizes = []
            vignetting = []

            for i in range(iq.nb_angles()):
                # Calculate field angle
                if iq.nb_angles() == 1:
                    angle = 0.0
                else:
                    angle = (i / (iq.nb_angles() - 1)) * self.optical_device.half_field_of_view()

                angles.append(angle)
                spot_sizes.append(iq.spot_size[i])
                vignetting.append(iq.vdVignetting[i])

                # Update table
                self.iq_table.setItem(i, 0, QTableWidgetItem(f"{angle:.3f}"))
                self.iq_table.setItem(i, 1, QTableWidgetItem(f"{iq.spot_size[i]:.3f}"))
                self.iq_table.setItem(i, 2, QTableWidgetItem(f"{iq.vdSpotvsAiry[i]:.3f}"))
                self.iq_table.setItem(i, 3, QTableWidgetItem(f"{iq.vdVignetting[i]:.1f}"))

            # Update results text with statistics
            if spot_sizes:
                results_text += f"Spot size range: {min(spot_sizes):.3f} - {max(spot_sizes):.3f} μm\n"
                results_text += f"Average spot size: {sum(spot_sizes)/len(spot_sizes):.3f} μm\n"
                results_text += f"Vignetting range: {min(vignetting):.1f} - {max(vignetting):.1f} %\n"

            self.results_text.setPlainText(results_text)

            # Update plot
            if angles and spot_sizes and vignetting:
                title = f"{os.path.basename(self.current_file)} - Spot Size vs Field Angle"
                self.plot_canvas.plot_spot_size_vs_angle(angles, spot_sizes, vignetting, title)

            self.status_bar.showMessage("Analysis complete")

        except Exception as e:
            QMessageBox.critical(self, "Analysis Error", f"Error during analysis: {str(e)}")
            self.status_bar.showMessage("Analysis failed")

        finally:
            self.progress_bar.setVisible(False)

    def show_about(self):
        """Show about dialog."""
        about_text = """
        <h2>Astree Python</h2>
        <p>Optical Design Analysis Tool</p>
        <p>A Python port of the Astree optical design application.</p>
        <p>Provides tools for loading optical designs, analyzing image quality,
        and visualizing optical performance.</p>
        <p>Built with PyQt and matplotlib.</p>
        """
        QMessageBox.about(self, "About Astree Python", about_text)

def main():
    """Main application entry point."""
    app = QApplication(sys.argv)
    app.setApplicationName("Astree Python")
    app.setApplicationVersion("1.0")

    # Create and show main window
    window = AstreeMainWindow()
    window.show()

    # Load file from command line if provided
    if len(sys.argv) > 1:
        filename = sys.argv[1]
        if os.path.exists(filename):
            window.load_design_file(filename)

    sys.exit(app.exec())

if __name__ == "__main__":
    main()