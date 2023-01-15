import math
from optical_device import OpticalDevice
from device_optimizer import DeviceOptimizer

def is_near(a, b, tolerance=1e-3):
    return abs(a - b) < tolerance

def test(bTest, sMessage):
    if bTest:
        return
    print("Test failed:", sMessage)
    exit(-1)

def test_parabolic_mirror():
    print("Test parabolic mirror start...")
    od = OpticalDevice()
    dDiameter = 200.0
    dFocal = 1000.0

    # surface0 = parabolic mirror
    od.insert_surface(0)
    od.set_type(0, "reflect")
    od.set(0, OpticalDevice.DIAMETER, dDiameter)
    od.set(0, OpticalDevice.RADIUS_CURVATURE, -2.0 * dFocal)
    od.set(0, OpticalDevice.CONIC, -1.0)
    od.set(0, OpticalDevice.THICK, -dFocal)

    # surface1 = image
    od.insert_surface(1)
    od.set_type(1, "image")
    od.set(1, OpticalDevice.AUTO_DIAMETER, 1)

    iq = od.get_image_quality()

    # For fast parabolic mirror; FNumber formula is:
    fNumber = dFocal / dDiameter

    print(f"Expected FNumber: {fNumber}")
    print(f"Actual FNumber: {iq.dFNumber}")
    print(f"Spot size: {iq.spot_size[0]}")
    print(f"Spot vs Airy: {iq.vdSpotvsAiry[0]}")
    print(f"Vignetting: {iq.vdVignetting[0]}")

    test(is_near(fNumber, iq.dFNumber), "FNumber must be equal")
    test(is_near(0, iq.spot_size[0]), "Spot size must be zero")
    test(is_near(0, iq.vdSpotvsAiry[0]), "Spot size vs Airy must be zero")
    test(is_near(100, iq.vdVignetting[0]), "Vignetting must be 100%")
    print("Test parabolic mirror ok.")

def test_computation_time():
    print("Test Duration start...")
    iNbTests = 100
    import time
    T1 = time.time()

    for iTest in range(iNbTests):
        od = OpticalDevice()
        dDiameter = 200.0
        dFocal = 1000.0

        # surface0 = parabolic mirror
        od.insert_surface(0)
        od.set_type(0, "reflect")
        od.set(0, OpticalDevice.DIAMETER, dDiameter)
        od.set(0, OpticalDevice.RADIUS_CURVATURE, -2.0 * dFocal)
        od.set(0, OpticalDevice.CONIC, -1.0)
        od.set(0, OpticalDevice.THICK, -dFocal)

        # surface1 = image
        od.insert_surface(1)
        od.set_type(1, "image")
        od.set(1, OpticalDevice.AUTO_DIAMETER, 1)

        iq = od.get_image_quality()

        # For fast parabolic mirror; FNumber formula is:
        fNumber = dFocal / dDiameter

        test(is_near(fNumber, iq.dFNumber), "FNumber must be equal")
        test(is_near(0, iq.spot_size[0]), "Spot size must be zero")
        test(is_near(0, iq.vdSpotvsAiry[0]), "Spot size vs Airy must be zero")
        test(is_near(100, iq.vdVignetting[0]), "Vignetting must be 100%")

    T2 = time.time()
    delta = (T2 - T1) * 1000  # in ms
    print(f"Duration: {delta / iNbTests} ms")
    print("Test Duration ok.")

def test_optimize_conic():
    print("Test optimize conic start...")
    od = OpticalDevice()
    dDiameter = 200.0
    dFocal = 1000.0

    # surface0 = parabolic mirror, but start with conic = 0 (spherical)
    od.insert_surface(0)
    od.set_type(0, "reflect")
    od.set(0, OpticalDevice.DIAMETER, dDiameter)
    od.set(0, OpticalDevice.RADIUS_CURVATURE, -2.0 * dFocal)
    od.set(0, OpticalDevice.CONIC, 0.0)  # start with spherical
    od.set(0, OpticalDevice.THICK, -dFocal)

    # surface1 = image
    od.insert_surface(1)
    od.set_type(1, "image")
    od.set(1, OpticalDevice.AUTO_DIAMETER, 1)

    # Optimize conic using Amoeba optimizer
    optimizer = DeviceOptimizer()
    optimizer.set_device(od)
    optimizer.add_parameter(0, "Conic", -2.0, 0.0)  # search from -2 to 0
    result = optimizer.optimize_amoeba()

    print(f"Optimization result: {result}")

    # Check the optimized conic
    optimized_conic = od.get(0, OpticalDevice.CONIC)
    print(f"Optimized conic: {optimized_conic}")

    # Check that it's close to -1
    test(is_near(-1.0, optimized_conic, 0.01), "Conic should be close to -1")

    # Check the spot size after optimization
    iq = od.get_image_quality()
    print(f"Spot size after optimization: {iq.spot_size[0]}")
    test(iq.spot_size[0] < 0.1, "Spot size should be very small after optimization")
    print("Test optimize conic ok.")

if __name__ == "__main__":
    test_parabolic_mirror()
    test_computation_time()
    test_optimize_conic()
    print("All tests ok.")