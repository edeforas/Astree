#!/usr/bin/env python3
"""
Astree Python Version - Complete Optical Design Software

This is a complete Python port of the Astree optical design software,
originally written in C++. The software provides optical ray tracing,
design optimization, and a full GUI for optical system analysis.

Features:
- Complete optical ray tracing engine
- Multiple optimization algorithms (Nelder-Mead/Amoeba)
- Design file I/O (.astree format)
- Image quality analysis (RMS spot size, vignetting)
- Interactive GUI with plotting capabilities
- Support for various optical surfaces and glasses

Usage:
    python astree_gui.py                    # Start GUI
    python astree_gui.py design.astree      # Load design and start GUI
    python test_newton.py                   # Run Newton test
    python test_scaling.py                  # Run scaling test
    python test_design_loading.py           # Test design loading
    python test_optimization.py             # Test optimization
    python test_gui.py                      # Test GUI functionality

Dependencies:
- numpy: For numerical computations
- matplotlib: For plotting (GUI only)
- PyQt6 or PyQt5: For GUI

Install dependencies:
    pip install numpy matplotlib PyQt6

Files:
- optical_device.py: Core optical system and ray tracing
- surface.py: Optical surface definitions
- photon.py: Light ray/photon implementation
- glass.py: Glass material properties
- device_optimizer.py: Optimization algorithms
- device_io.py: Design file I/O
- astree_gui.py: Main GUI application
- test_*.py: Various test scripts

The Python version maintains full compatibility with the original C++
.astree design files and provides the same optical analysis capabilities.
"""

if __name__ == "__main__":
    print(__doc__)