import math
import matplotlib.pyplot as plt
import numpy as np
from optical_device import OpticalDevice
from device_io import load_device

def load_schmidt_camera(filepath):
    """
    Load a real Schmidt camera design from an .astree file.
    
    A Schmidt camera system consists of:
    1. BK7 aspheric corrector plate (refractive)
    2. Air gap
    3. Spherical mirror (reflective)
    4. Image surface at the focal plane
    
    This design is from: http://www.telescope-optics.net/appendix3.htm (Model 2: Schmidt Camera)
    """
    od = load_device(filepath)
    
    if od is None:
        print(f"Error: Failed to load design from {filepath}")
        return None, None, None, None
    
    # Extract basic parameters from the design
    dHalfFov = od.half_field_of_view()
    
    # Increase number of angles for finer resolution across the field
    od.set_nb_angles(13)
    
    # Set primary light colors for chromatic analysis
    # RGB: Red, Green, Blue wavelengths
    od.set_light_colors("Red.Green.Blue.")
    
    # Estimate focal length from mirror radius (Surface 2)
    if od.nb_surface() > 2:
        dMirrorRadius = od.get(2, OpticalDevice.RADIUS_CURVATURE)
        dFocal = -dMirrorRadius / 2.0 if dMirrorRadius != 0 else 1000.0
    else:
        dFocal = 1000.0
    
    # Get aperture from Surface 0 (corrector plate)
    dDiameter = od.get(0, OpticalDevice.DIAMETER)
    
    return od, dHalfFov, dFocal, dDiameter



def plot_schmidt_image_quality(schmidt_filepath):
    """
    Load a real Schmidt camera design and plot the image quality from center to edge.
    Shows how image quality (spot size) varies across the field of view.
    """
    print("Loading Schmidt camera system...")
    od, dHalfFov, dFocal, dDiameter = load_schmidt_camera(schmidt_filepath)
    
    if od is None:
        return
    
    print(f"\nSchmidt Camera Parameters:")
    print(f"  Aperture diameter: {dDiameter} mm")
    print(f"  Focal length: {dFocal} mm")
    print(f"  F-number: {dFocal/dDiameter:.1f}")
    print(f"  Field of view: {2*dHalfFov:.1f}°")
    print(f"  Number of surfaces: {od.nb_surface()}")
    
    print("\nDesign Summary:")
    for i in range(od.nb_surface()):
        surf_type = od.type(i)
        diameter = od.get(i, OpticalDevice.DIAMETER)
        radius = od.get(i, OpticalDevice.RADIUS_CURVATURE)
        thick = od.get(i, OpticalDevice.THICK)
        print(f"  Surface {i}: {surf_type:8s}, D={diameter:8.1f}mm, R={radius:10.1f}mm, T={thick:10.3f}mm")
    
    print("\nComputing image quality across the field...")
    iq = od.get_image_quality()
    
    print(f"F-Number: {iq.dFNumber:.2f}")
    print(f"Airy disk size: {iq.dAirySize:.6f} µm")
    print(f"Number of field angles: {iq.nb_angles()}")
    
    # Diagnostic information about wavelengths and Airy disk calculation
    print(f"\nLight colors configured: {od.light_colors()}")
    print(f"Expected Airy disk size (standard calculation):")
    
    # Standard Airy disk calculation for visible light
    # Airy disk diameter = 1.22 * lambda / (2 * F#)
    lambda_red = 650    # nm (Red)
    lambda_green = 550  # nm (Green) 
    lambda_blue = 450   # nm (Blue)
    
    airy_red = 1.22 * lambda_red / (2 * iq.dFNumber)
    airy_green = 1.22 * lambda_green / (2 * iq.dFNumber)
    airy_blue = 1.22 * lambda_blue / (2 * iq.dFNumber)
    
    print(f"  Red (650 nm):   {airy_red:.6f} nm")
    print(f"  Green (550 nm): {airy_green:.6f} nm")
    print(f"  Blue (450 nm):  {airy_blue:.6f} nm")
    print(f"  Average:        {(airy_red + airy_green + airy_blue)/3:.6f} nm")
    print(f"\nActual Airy disk from system: {iq.dAirySize:.6f} nm")
    print(f"Ratio (Actual/Expected Green): {iq.dAirySize / airy_green if airy_green > 0 else 0:.6f}")
    
    # Extract data for plotting, with validation
    angles = []
    spot_sizes = []
    spot_vs_airy = []
    vignetting = []
    
    print("\nImage Quality vs Field Angle:")
    print("-" * 90)
    print(f"{'Angle (°)':>12} {'Spot Size (µm)':>18} {'Spot vs Airy':>18} {'Vignetting (%)':>18}")
    print("-" * 90)
    
    # Helper function to handle numeric validation
    def is_valid_number(val):
        return not (math.isnan(val) or math.isinf(val))
    
    for i in range(iq.nb_angles()):
        # Calculate field angle in degrees
        if iq.nb_angles() == 1:
            angle = 0.0
        else:
            angle = (i / (iq.nb_angles() - 1)) * dHalfFov
        
        angles.append(angle)
        
        # Validate spot size
        spot_size = iq.spot_size[i] if is_valid_number(iq.spot_size[i]) else 0.0
        spot_sizes.append(spot_size)
        
        # Validate spot vs Airy
        spot_vs_a = iq.vdSpotvsAiry[i] if is_valid_number(iq.vdSpotvsAiry[i]) else 0.0
        spot_vs_airy.append(spot_vs_a)
        
        # Validate vignetting
        vig = iq.vdVignetting[i] if iq.vdVignetting[i] >= 0 else 0.0
        vignetting.append(vig)
        
        print(f"{angle:12.2f} {spot_size:18.6f} {spot_vs_a:18.6f} {vig:18.1f}")
    
    print("-" * 90)
    
    # Summary statistics (filtered for valid values)
    valid_spots = [s for s in spot_sizes if s > 0]
    if valid_spots:
        print("\nSummary Statistics:")
        print(f"  Spot size range: {min(valid_spots):.3f} - {max(valid_spots):.3f} µm")
        print(f"  Average spot size: {np.mean(valid_spots):.3f} µm")
        print(f"  Spot size at edge: {spot_sizes[-1] if spot_sizes[-1] > 0 else 'N/A'}")
    else:
        print("\nNote: Optical design may need refinement for valid spot size data.")
    
    valid_vig = [v for v in vignetting if v >= 0]
    if valid_vig:
        print(f"  Vignetting range: {min(valid_vig):.1f} - {max(valid_vig):.1f} %")
    
    # Create plots
    print("\nGenerating plots...")
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Schmidt Camera System - Image Quality Analysis (Multi-wavelength, Multi-angle)', fontsize=16, fontweight='bold')
    
    # Create color map for field angles
    colors = plt.cm.viridis(np.linspace(0, 1, len(angles)))
    
    # Plot 1: Spot size vs field angle with color gradient
    ax1 = axes[0, 0]
    scatter1 = ax1.scatter(angles, spot_sizes, c=angles, cmap='cool', s=120, edgecolors='black', linewidth=1.5, alpha=0.8)
    ax1.plot(angles, spot_sizes, 'k--', linewidth=1.5, alpha=0.4)
    
    if iq.dAirySize > 0 and iq.dAirySize < 1e10 and is_valid_number(iq.dAirySize):
        ax1.axhline(y=iq.dAirySize, color='r', linestyle='--', alpha=0.7, linewidth=2.5, 
                   label=f'Airy disk ({iq.dAirySize:.6f} µm)')
    ax1.set_xlabel('Field Angle (degrees)', fontsize=11, fontweight='bold')
    ax1.set_ylabel('RMS Spot Size (µm)', fontsize=11, fontweight='bold')
    ax1.set_title('Spot Size vs Field Angle', fontsize=12, fontweight='bold')
    ax1.grid(True, alpha=0.3, linestyle=':')
    cbar1 = plt.colorbar(scatter1, ax=ax1)
    cbar1.set_label('Field Angle (°)', fontsize=10)
    if iq.dAirySize > 0 and iq.dAirySize < 1e10:
        ax1.legend(fontsize=10, loc='upper left')
    
    # Plot 2: Spot size vs Airy ratio with color gradient
    ax2 = axes[0, 1]
    scatter2 = ax2.scatter(angles, spot_vs_airy, c=angles, cmap='plasma', s=120, edgecolors='black', linewidth=1.5, alpha=0.8)
    ax2.plot(angles, spot_vs_airy, 'k--', linewidth=1.5, alpha=0.4)
    ax2.axhline(y=1.0, color='lime', linestyle='--', alpha=0.7, linewidth=2.5, label='Diffraction limit (1.0x)')
    ax2.set_xlabel('Field Angle (degrees)', fontsize=11, fontweight='bold')
    ax2.set_ylabel('Spot Size / Airy Disk', fontsize=11, fontweight='bold')
    ax2.set_title('Diffraction Performance vs Field Angle', fontsize=12, fontweight='bold')
    ax2.grid(True, alpha=0.3, linestyle=':')
    cbar2 = plt.colorbar(scatter2, ax=ax2)
    cbar2.set_label('Field Angle (°)', fontsize=10)
    ax2.legend(fontsize=10, loc='upper left')
    
    # Plot 3: Vignetting vs field angle with color gradient
    ax3 = axes[1, 0]
    scatter3 = ax3.scatter(angles, vignetting, c=angles, cmap='RdYlGn', s=120, edgecolors='black', linewidth=1.5, alpha=0.8)
    ax3.plot(angles, vignetting, 'k--', linewidth=1.5, alpha=0.4)
    if valid_vig:
        ax3.axhline(y=100.0, color='darkgreen', linestyle='--', alpha=0.7, linewidth=2.5, label='Full transmission (100%)')
        ax3.set_ylim([max(0, min(valid_vig)-5), 105])
    ax3.set_xlabel('Field Angle (degrees)', fontsize=11, fontweight='bold')
    ax3.set_ylabel('Vignetting (%)', fontsize=11, fontweight='bold')
    ax3.set_title('Vignetting vs Field Angle', fontsize=12, fontweight='bold')
    ax3.grid(True, alpha=0.3, linestyle=':')
    cbar3 = plt.colorbar(scatter3, ax=ax3)
    cbar3.set_label('Field Angle (°)', fontsize=10)
    if valid_vig:
        ax3.legend(fontsize=10)
    
    # Plot 4: Summary table with enhanced information
    ax4 = axes[1, 1]
    ax4.axis('off')
    
    summary_data = [
        ['Parameter', 'Value'],
        ['Aperture Diameter', f'{dDiameter:.1f} mm'],
        ['Focal Length', f'{dFocal:.1f} mm'],
        ['F-Number', f'{iq.dFNumber:.2f}'],
        ['Field of View', f'{2*dHalfFov:.2f}°'],
        ['Airy Disk Size', f'{iq.dAirySize:.6f} µm' if is_valid_number(iq.dAirySize) and 0 < iq.dAirySize < 1e10 else 'N/A'],
        ['Center Spot Size', f'{spot_sizes[0]:.6f} µm' if spot_sizes[0] > 0 else 'N/A'],
        ['Edge Spot Size', f'{spot_sizes[-1]:.6f} µm' if spot_sizes[-1] > 0 else 'N/A'],
        ['Max Vignetting Loss', f'{100-min(valid_vig) if valid_vig else 0:.1f}%'],
        ['Field Angles', f'{len(angles)} points'],
        ['Wavelengths', 'Red, Green, Blue'],
    ]
    
    table = ax4.table(cellText=summary_data, cellLoc='left', loc='center',
                     colWidths=[0.5, 0.5])
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1, 1.8)
    
    # Format header row
    for i in range(len(summary_data[0])):
        table[(0, i)].set_facecolor('#40466e')
        table[(0, i)].set_text_props(weight='bold', color='white')
    
    # Format data rows with alternating colors
    for i in range(1, len(summary_data)):
        if i % 2 == 0:
            table[(i, 0)].set_facecolor('#e8f4f8')
            table[(i, 1)].set_facecolor('#ffffff')
        else:
            table[(i, 0)].set_facecolor('#f0f0f0')
            table[(i, 1)].set_facecolor('#fafafa')
    
    plt.tight_layout()
    
    # Save the plot without displaying it interactively
    output_path = "schmidt_camera_analysis.png"
    plt.savefig(output_path, dpi=150, bbox_inches='tight')
    print(f"\nPlot saved to: {output_path}")
    plt.close(fig)
    
    print("Schmidt camera analysis complete!")



if __name__ == "__main__":
    schmidt_filepath = "c:/dev/Astree/samples/telescopes/Schmidt.astree"
    plot_schmidt_image_quality(schmidt_filepath)
