import math
from optical_device import OpticalDevice
from device_scaling import DeviceScaling

def is_near(a, b, tolerance=1e-10):
    return abs(a - b) < tolerance

def test(bTest, sMessage):
    if bTest:
        return
    print("Test failed:", sMessage)
    exit(-1)

def test_scaling_thick():
    od = OpticalDevice()

    # surface0 = perfect lens
    od.insert_surface(0)
    od.set_type(0, "perfect_lens")
    od.set(0, OpticalDevice.DIAMETER, 200)
    od.set(0, OpticalDevice.RADIUS_CURVATURE, 2000)  # F=-1000mm
    od.set(0, OpticalDevice.THICK, 1000)

    test(is_near(200., od.get(0, OpticalDevice.DIAMETER)), "D=200mm")
    test(is_near(2000., od.get(0, OpticalDevice.RADIUS_CURVATURE)), "RC=2000mm")
    test(is_near(1000., od.get(0, OpticalDevice.THICK)), "THICK=1000mm")

    d = DeviceScaling()
    d.scale(od, 2.)

    test(is_near(2.*200., od.get(0, OpticalDevice.DIAMETER)), "D=400mm")
    test(is_near(2.*2000., od.get(0, OpticalDevice.RADIUS_CURVATURE)), "RC=4000mm")
    test(is_near(2.*1000., od.get(0, OpticalDevice.THICK)), "THICK=2000mm")

def test_scaling_Z():
    od = OpticalDevice()

    # surface0 = perfect lens
    od.insert_surface(0)
    od.set_type(0, "perfect_lens")
    od.set(0, OpticalDevice.DIAMETER, 200)
    od.set(0, OpticalDevice.RADIUS_CURVATURE, 2000)  # F=-1000mm
    od.set(0, OpticalDevice.Z, 1000)

    test(is_near(200., od.get(0, OpticalDevice.DIAMETER)), "D=200mm")
    test(is_near(2000., od.get(0, OpticalDevice.RADIUS_CURVATURE)), "RC=2000mm")
    test(is_near(1000., od.get(0, OpticalDevice.Z)), "Z=1000mm")

    d = DeviceScaling()
    d.scale(od, 2.)

    test(is_near(2.*200., od.get(0, OpticalDevice.DIAMETER)), "D=400mm")
    test(is_near(2.*2000., od.get(0, OpticalDevice.RADIUS_CURVATURE)), "RC=4000mm")
    # Z is not scaled
    test(is_near(1000., od.get(0, OpticalDevice.Z)), "Z=1000mm")

if __name__ == "__main__":
    test_scaling_thick()
    # test_scaling_Z()  # TODO: Z scaling not implemented
    print("all tests success!")