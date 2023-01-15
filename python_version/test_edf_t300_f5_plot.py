import math
import matplotlib.pyplot as plt
from optical_device import OpticalDevice
from device_io import load_device

def is_near(a, b, tolerance=1e-3):
    return abs(a - b) < tolerance

def test(bTest, sMessage):
    if bTest:
        return
    print("Test failed:", sMessage)
    exit(-1)

def test_edf_t300_f5_plot():
    print("Test load EdF T300 F5 design and plot spot size vs angle...")

    # Load the EdF T300 F5 telescope design
    od = load_device("c:/dev/Astree/samples/telescopes/EdF_T300_F5.astree")

    test(od is not None, "Failed to load EdF T300 F5 design")

    print(f"Number of surfaces: {od.nb_surface()}")

    # Check basic properties
    test(od.nb_surface() == 8, "Should have 8 surfaces")

    # Check light settings
    test(is_near(od.half_field_of_view(), 0.55, 0.01), "Half field of view")
    test(od.nb_intermediate_angles() == 3, "Number of intermediate angles")

    print("Design loaded successfully.")

    # Print design summary
    print("\nDesign Summary:")
    for i in range(od.nb_surface()):
        surf_type = od.type(i)
        diameter = od.get(i, OpticalDevice.DIAMETER)
        radius = od.get(i, OpticalDevice.RADIUS_CURVATURE)
        thick = od.get(i, OpticalDevice.THICK)
        print(f"Surface {i}: {surf_type}, D={diameter:.1f}mm, R={radius:.1f}mm, T={thick:.3f}mm")

    print(f"\nLight settings: Half FOV = {od.half_field_of_view()}°, {od.nb_intermediate_angles()} field points")

    # Compute image quality
    print("\nComputing image quality...")
    iq = od.get_image_quality()

    print(f"F-Number: {iq.dFNumber}")
    print(f"Airy disk size: {iq.dAirySize}")
    print(f"Number of field angles: {iq.nb_angles()}")

    # Extract data for plotting
    angles = []
    spot_sizes = []
    vignetting = []

    for i in range(iq.nb_angles()):
        # Calculate field angle in degrees
        if iq.nb_angles() == 1:
            angle = 0.0
        else:
            angle = (i / (iq.nb_angles() - 1)) * od.half_field_of_view()

        angles.append(angle)
        spot_sizes.append(iq.spot_size[i])
        vignetting.append(iq.vdVignetting[i])

        print(f"Angle {angle:.3f}°: Spot size = {iq.spot_size[i]:.6f}, "
              f"Spot vs Airy = {iq.vdSpotvsAiry[i]:.6f}, "
              f"Vignetting = {iq.vdVignetting[i]:.1f}%")

    # Basic checks
    test(iq.nb_angles() > 0, "Should have field angles")
    test(iq.dFNumber > 0, "F-Number should be positive")
    test(all(not math.isnan(s) for s in spot_sizes), "All spot sizes should be valid")
    test(all(not math.isinf(s) for s in spot_sizes), "All spot sizes should be finite")

    print("\nImage quality computed successfully.")

    # Print summary statistics
    print("\nSummary Statistics:")
    print(f"Spot size range: {min(spot_sizes):.3f} - {max(spot_sizes):.3f} μm")
    print(f"Average spot size: {sum(spot_sizes)/len(spot_sizes):.3f} μm")
    print(f"Vignetting range: {min(vignetting):.1f} - {max(vignetting):.1f} %")

    # Create plot
    try:
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))

        # Plot spot size vs angle
        ax1.plot(angles, spot_sizes, 'bo-', linewidth=2, markersize=8)
        ax1.set_xlabel('Field Angle (degrees)')
        ax1.set_ylabel('Spot Size (μm)')
        ax1.set_title('EdF T300 F5 Telescope - Spot Size vs Field Angle')
        ax1.grid(True, alpha=0.3)

        # Add Airy disk reference line if reasonable
        if 0 < iq.dAirySize < 1e10:
            ax1.axhline(y=iq.dAirySize, color='r', linestyle='--', alpha=0.7,
                       label=f'Airy Disk ({iq.dAirySize:.1f} μm)')
            ax1.legend()

        # Plot vignetting vs angle
        ax2.plot(angles, vignetting, 'ro-', linewidth=2, markersize=8)
        ax2.set_xlabel('Field Angle (degrees)')
        ax2.set_ylabel('Vignetting (%)')
        ax2.set_title('Vignetting vs Field Angle')
        ax2.grid(True, alpha=0.3)
        ax2.set_ylim(0, 105)

        plt.tight_layout()
        plt.savefig('edf_t300_f5_spot_size_plot.png', dpi=150, bbox_inches='tight')
        print("Plot saved as 'edf_t300_f5_spot_size_plot.png'")

        # Don't show plot in automated environment
        # plt.show()

    except ImportError:
        print("Matplotlib not available, skipping plot generation")
        print("Data summary:")
        for i, (angle, spot, vig) in enumerate(zip(angles, spot_sizes, vignetting)):
            print(f"  {angle:.3f}°: {spot:.3f} μm, {vig:.1f}% vignetting")

    print("Test load EdF T300 F5 and plot ok.")

if __name__ == "__main__":
    test_edf_t300_f5_plot()
    print("All tests ok.")