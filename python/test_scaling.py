import math
import sys

class OpticalDevice:
    # Placeholder for the OpticalDevice class implementation
    def __init__(self):
        self.surfaces = {}

    def insert_surface(self, index):
        self.surfaces[index] = {}

    def set_type(self, index, type_name):
        self.surfaces[index]['type'] = type_name

    def set(self, index, property_name, value):
        self.surfaces[index][property_name] = value

    def get(self, index, property_name):
        return self.surfaces[index].get(property_name, None)

class DeviceScaling:
    def scale(self, optical_device, factor):
        for surface in optical_device.surfaces.values():
            for key in surface.keys():
                if key in [DIAMETER, RADIUS_CURVATURE, THICK, Z]:
                    surface[key] *= factor

DIAMETER = 'DIAMETER'
RADIUS_CURVATURE = 'RADIUS_CURVATURE'
THICK = 'THICK'
Z = 'Z'

def is_near(a, b, tolerance=1e-10):
    return abs(a - b) < tolerance

def test(bTest, sMessage):
    if bTest:
        return
    print(f"Test failed: {sMessage}")
    sys.exit(-1)

def test_scaling_thick():
    od = OpticalDevice()

    # surface0 = perfect lens
    od.insert_surface(0)
    od.set_type(0, "perfect_lens")
    od.set(0, DIAMETER, 200)
    od.set(0, RADIUS_CURVATURE, 2000)  # F=-1000mm
    od.set(0, THICK, 1000)

    test(is_near(200., od.get(0, DIAMETER)), "D=200mm")
    test(is_near(2000., od.get(0, RADIUS_CURVATURE)), "RC=2000mm")
    test(is_near(1000., od.get(0, THICK)), "THICK=1000mm")

    d = DeviceScaling()
    d.scale(od, 2.)

    test(is_near(2. * 200., od.get(0, DIAMETER)), "D=400mm")
    test(is_near(2. * 2000., od.get(0, RADIUS_CURVATURE)), "RC=4000mm")
    test(is_near(2. * 1000., od.get(0, THICK)), "THICK=2000mm")

def test_scaling_Z():
    od = OpticalDevice()

    # surface0 = perfect lens
    od.insert_surface(0)
    od.set_type(0, "perfect_lens")
    od.set(0, DIAMETER, 200)
    od.set(0, RADIUS_CURVATURE, 2000)  # F=-1000mm
    od.set(0, Z, 1000)

    test(is_near(200., od.get(0, DIAMETER)), "D=200mm")
    test(is_near(2000., od.get(0, RADIUS_CURVATURE)), "RC=2000mm")
    test(is_near(1000., od.get(0, Z)), "Z=1000mm")

    d = DeviceScaling()
    d.scale(od, 2.)

    test(is_near(2. * 200., od.get(0, DIAMETER)), "D=400mm")
    test(is_near(2. * 2000., od.get(0, RADIUS_CURVATURE)), "RC=4000mm")
    # TODO DEBUG    test(is_near(2.*1000., od.get(0, Z)), "Z=2000mm")

if __name__ == "__main__":
    test_scaling_thick()
    test_scaling_Z()

    print("all tests success!")