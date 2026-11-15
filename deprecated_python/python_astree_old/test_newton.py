import math
import time

import OpticalDevice
import ImageQuality


def is_near(a, b, tolerance=1.e-3):
    return abs(a - b) < tolerance

def test(bTest, sMessage):
    if bTest:
        return
    print(f"Test failed: {sMessage}")
    exit(-1)

def test_parabolic_mirror():
    print("Test parabolic mirror start...")
    od = OpticalDevice.OpticalDevice()
    dDiameter = 200.0
    dFocal = 1000.0

    # surface0 = parabolic mirror
    od.insert_surface(0)
    od.set_type(0, "reflect")
    od.set(0, "DIAMETER", dDiameter)
    od.set(0, "RADIUS_CURVATURE", -2.0 * dFocal)
    od.set(0, "CONIC", -1.0)
    od.set(0, "THICK", -dFocal)

    # surface1 = image
    od.insert_surface(1)
    od.set_type(1, "image")
    od.set(1, "AUTO_DIAMETER", 1)

    iq = od.get_image_quality()

    # for fast parabolic mirror ; FNumber formula is :
    fNumber = dFocal / dDiameter - dDiameter / dFocal / 16.0

    test(is_near(fNumber, iq.dFNumber), "FNumber must be equal")
    test(is_near(0, iq.vdSpotSize[0]), "Spot size must be zero")
    test(is_near(0, iq.vdSpotvsAiry[0]), "Spot size vs Airy must be zero")
    test(is_near(100, iq.vdVignetting[0]), "Vignetting must be 100%")
    print("Test parabolic mirror ok.")

def test_computation_time():
    print("Test Duration start...")
    iNbTests = 100
    T1 = time.time()

    for iTest in range(iNbTests):
        od = OpticalDevice()
        dDiameter = 200.0
        dFocal = 1000.0

        # surface0 = parabolic mirror
        od.insert_surface(0)
        od.set_type(0, "reflect")
        od.set(0, "DIAMETER", dDiameter)
        od.set(0, "RADIUS_CURVATURE", -2.0 * dFocal)
        od.set(0, "CONIC", -1.0)
        od.set(0, "THICK", -dFocal)

        # surface1 = image
        od.insert_surface(1)
        od.set_type(1, "image")
        od.set(1, "AUTO_DIAMETER", 1)

        iq = od.get_image_quality()

        # for fast parabolic mirror ; FNumber formula is :
        fNumber = dFocal / dDiameter - dDiameter / dFocal / 16.0

        test(is_near(fNumber, iq.dFNumber), "FNumber must be equal")
        test(is_near(0, iq.vdSpotSize[0]), "Spot size must be zero")
        test(is_near(0, iq.vdSpotvsAiry[0]), "Spot size vs Airy must be zero")
        test(is_near(100, iq.vdVignetting[0]), "Vignetting must be 100%")

    T2 = time.time()
    delta = (T2 - T1) * 1000  # convert to milliseconds
    print(f"Duration: {delta / iNbTests} ms")
    print("Test Duration ok.")

if __name__ == "__main__":
    test_parabolic_mirror()
    test_computation_time()
    print("All tests ok.")