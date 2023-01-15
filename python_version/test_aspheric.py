import math
from optical_device import OpticalDevice
import numpy as np


def is_near(a, b, tolerance=1e-3):
    return abs(a - b) < tolerance


def test(bTest, sMessage):
    if bTest:
        return
    print("Test failed:", sMessage)
    exit(-1)


def test_spherical_mirror():
    """Test spherical mirror (baseline)"""
    print("Test spherical mirror start...")
    od = OpticalDevice()
    dDiameter = 200.0
    dFocal = 1000.0

    # surface0 = spherical mirror
    od.insert_surface(0)
    od.set_type(0, "reflect")
    od.set(0, OpticalDevice.DIAMETER, dDiameter)
    od.set(0, OpticalDevice.RADIUS_CURVATURE, -2.0 * dFocal)
    od.set(0, OpticalDevice.CONIC, 0.0)  # spherical
    od.set(0, OpticalDevice.THICK, -dFocal)

    # surface1 = image
    od.insert_surface(1)
    od.set_type(1, "image")
    od.set(1, OpticalDevice.AUTO_DIAMETER, 1)

    iq = od.get_image_quality()
    print(f"Spherical mirror spot size: {iq.spot_size[0]}")
    test(iq.spot_size[0] > 0.5, "Spherical mirror should have aberrations")
    print("Test spherical mirror ok.")


def test_aspheric_correction():
    """Test aspheric surface with polynomial correction"""
    print("\nTest aspheric correction start...")
    od = OpticalDevice()
    dDiameter = 200.0
    dFocal = 1000.0

    # surface0 = mirror with aspheric correction
    od.insert_surface(0)
    od.set_type(0, "reflect")
    od.set(0, OpticalDevice.DIAMETER, dDiameter)
    od.set(0, OpticalDevice.RADIUS_CURVATURE, -2.0 * dFocal)
    od.set(0, OpticalDevice.CONIC, 0.0)  # spherical base
    
    # Add small aspheric correction (R4 term)
    # R4 coefficient should be very small for slight correction
    dR4 = -1e-9  # Small correction
    od.set(0, OpticalDevice.R4, dR4)
    od.set(0, OpticalDevice.THICK, -dFocal)

    # surface1 = image
    od.insert_surface(1)
    od.set_type(1, "image")
    od.set(1, OpticalDevice.AUTO_DIAMETER, 1)

    iq = od.get_image_quality()
    print(f"Aspheric mirror (R4={dR4}) spot size: {iq.spot_size[0]}")
    test(not math.isnan(iq.spot_size[0]), "Spot size should be valid")
    test(not math.isinf(iq.spot_size[0]), "Spot size should not be infinite")
    print("Test aspheric correction ok.")


def test_polynomial_parameters():
    """Test setting/getting polynomial coefficients"""
    print("\nTest polynomial parameters start...")
    od = OpticalDevice()

    od.insert_surface(0)
    od.set_type(0, "reflect")

    # Test R4
    od.set(0, OpticalDevice.R4, 1e-10)
    r4 = od.get(0, OpticalDevice.R4)
    test(is_near(r4, 1e-10, 1e-15), f"R4 should be 1e-10, got {r4}")

    # Test R6
    od.set(0, OpticalDevice.R6, 2e-10)
    r6 = od.get(0, OpticalDevice.R6)
    test(is_near(r6, 2e-10, 1e-15), f"R6 should be 2e-10, got {r6}")

    # Test R8
    od.set(0, OpticalDevice.R8, 3e-10)
    r8 = od.get(0, OpticalDevice.R8)
    test(is_near(r8, 3e-10, 1e-15), f"R8 should be 3e-10, got {r8}")

    # Test R10
    od.set(0, OpticalDevice.R10, 4e-10)
    r10 = od.get(0, OpticalDevice.R10)
    test(is_near(r10, 4e-10, 1e-15), f"R10 should be 4e-10, got {r10}")

    print("Test polynomial parameters ok.")


def test_compute_surface_profile():
    """Test compute_z method for surface sag"""
    print("\nTest compute surface profile start...")
    od = OpticalDevice()

    # Create a spherical mirror with known radius
    od.insert_surface(0)
    od.set_type(0, "reflect")
    od.set(0, OpticalDevice.DIAMETER, 100.0)
    od.set(0, OpticalDevice.RADIUS_CURVATURE, -1000.0)  # Radius = 1000
    od.set(0, OpticalDevice.CONIC, 0.0)  # Spherical

    # Add slight aspheric correction
    od.set(0, OpticalDevice.R4, -1e-10)

    # Test at different radii
    x, y = 0.0, 0.0
    z = od._vSurfaces[0].compute_z(x, y)
    print(f"Surface sag at (0,0): z={z}")
    test(z[1], "compute_z should return valid at center")

    x, y = 10.0, 0.0
    z = od._vSurfaces[0].compute_z(x, y)
    print(f"Surface sag at (10,0): z={z}")
    test(z[1], "compute_z should return valid at radius 10")

    x, y = 50.0, 0.0
    z = od._vSurfaces[0].compute_z(x, y)
    print(f"Surface sag at (50,0): z={z}")
    test(z[1], "compute_z should return valid at radius 50")

    print("Test compute surface profile ok.")


if __name__ == "__main__":
    test_spherical_mirror()
    test_aspheric_correction()
    test_polynomial_parameters()
    test_compute_surface_profile()
    print("\n" + "="*60)
    print("All aspheric tests passed!")
    print("="*60)
