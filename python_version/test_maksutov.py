import math
from optical_device import OpticalDevice
from device_io import load_device

def is_near(a, b, tolerance=1e-3):
    return abs(a - b) < tolerance

def test(bTest, sMessage):
    if bTest:
        return
    print("Test failed:", sMessage)
    exit(-1)

def test_load_maksutov():
    print("Test load Maksutov design start...")

    # Load the Maksutov telescope design
    od = load_device("c:/dev/Astree/samples/telescopes/Maksutov.astree")

    test(od is not None, "Failed to load Maksutov design")

    print(f"Number of surfaces: {od.nb_surface()}")

    # Check basic properties
    test(od.nb_surface() == 4, "Should have 4 surfaces")

    # Surface 0: BK7 lens
    test(od.type(0) == "BK7", f"Surface 0 should be BK7, got {od.type(0)}")
    test(is_near(od.get(0, OpticalDevice.DIAMETER), 200.0), "Surface 0 diameter")
    test(is_near(od.get(0, OpticalDevice.RADIUS_CURVATURE), -225.36), "Surface 0 radius")
    test(is_near(od.get(0, OpticalDevice.THICK), 21.0), "Surface 0 thickness")

    # Surface 1: Air
    test(od.type(1) == "Air", f"Surface 1 should be Air, got {od.type(1)}")
    test(is_near(od.get(1, OpticalDevice.DIAMETER), 207.6152509), "Surface 1 diameter")
    test(is_near(od.get(1, OpticalDevice.RADIUS_CURVATURE), -237.8), "Surface 1 radius")
    test(is_near(od.get(1, OpticalDevice.THICK), 670.0), "Surface 1 thickness")

    # Surface 2: Mirror
    test(od.type(2) == "reflect", f"Surface 2 should be reflect, got {od.type(2)}")
    test(is_near(od.get(2, OpticalDevice.DIAMETER), 257.180131), "Surface 2 diameter")
    test(is_near(od.get(2, OpticalDevice.RADIUS_CURVATURE), -1063.0), "Surface 2 radius")
    test(is_near(od.get(2, OpticalDevice.THICK), -545.3254278), "Surface 2 thickness")

    # Surface 3: Image
    test(od.type(3) == "image", f"Surface 3 should be image, got {od.type(3)}")
    test(is_near(od.get(3, OpticalDevice.DIAMETER), 32.00297665), "Surface 3 diameter")
    test(is_near(od.get(3, OpticalDevice.RADIUS_CURVATURE), -458.0088815), "Surface 3 radius")

    # Check light settings
    test(is_near(od.half_field_of_view(), 1.79), "Half field of view")
    test(od.nb_intermediate_angles() == 3, "Number of intermediate angles")

    print("Design loaded successfully.")

    # Compute image quality
    print("Computing image quality...")
    iq = od.get_image_quality()

    print(f"F-Number: {iq.dFNumber}")
    print(f"Airy disk size: {iq.dAirySize}")
    print(f"Number of field angles: {iq.nb_angles()}")

    for i in range(iq.nb_angles()):
        print(f"Angle {i}: Spot size = {iq.spot_size[i]:.6f}, "
              f"Spot vs Airy = {iq.vdSpotvsAiry[i]:.6f}, "
              f"Vignetting = {iq.vdVignetting[i]:.1f}%")

    # Basic checks
    test(iq.nb_angles() > 0, "Should have field angles")
    test(iq.dFNumber > 0, "F-Number should be positive")
    test(iq.dAirySize > 0, "Airy size should be positive")

    # Check that spot sizes are reasonable (not infinite)
    for i in range(iq.nb_angles()):
        test(not math.isnan(iq.spot_size[i]), f"Spot size {i} should not be NaN")
        test(not math.isinf(iq.spot_size[i]), f"Spot size {i} should not be infinite")
        test(iq.vdVignetting[i] >= 0, f"Vignetting {i} should be non-negative")

    print("Image quality computed successfully.")
    print("Test load Maksutov ok.")

if __name__ == "__main__":
    test_load_maksutov()
    print("All tests ok.")