import os
from optical_device import OpticalDevice

class Properties:
    def __init__(self):
        self._pairs = {}

    def load(self, filename):
        self._pairs.clear()
        try:
            with open(filename, 'r') as f:
                for line in f:
                    line = line.strip()
                    if not line:
                        continue
                    if '=' not in line:
                        continue
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    if key in self._pairs:
                        self._pairs[key] += '\n' + value
                    else:
                        self._pairs[key] = value
            return True
        except:
            return False

    def exist(self, key):
        return key in self._pairs

    def get(self, key):
        return self._pairs.get(key, "")

    def get_double(self, key):
        try:
            return float(self.get(key))
        except:
            return 0.0

    def get_int(self, key):
        try:
            return int(self.get(key))
        except:
            return 0

    def get_bool(self, key):
        val = self.get(key).lower()
        return val in ('1', 'true', 'yes')

def load_device(filename):
    prop = Properties()
    if not prop.load(filename):
        return None

    od = OpticalDevice()

    # Set relative convention
    if prop.exist("device.relative_convention"):
        od.set_relative_convention(prop.get_bool("device.relative_convention"))
    elif prop.exist("device.convention"):
        od.set_relative_convention(prop.get("device.convention") == "relative")
    else:
        od.set_relative_convention(False)

    # Load surfaces
    iS = 0
    while True:
        sSurfName = str(iS)
        sType = ""
        if prop.exist(sSurfName + ".type"):
            sType = prop.get(sSurfName + ".type")
        elif prop.exist(sSurfName + ".kind"):  # compatibility
            sType = prop.get(sSurfName + ".kind")

        if not sType:
            break

        if sType == "observe":
            sType = "image"  # conversion from old format

        od.insert_surface(iS)
        od.set_type(iS, sType)

        # Diameter
        if prop.exist(sSurfName + ".diameter"):
            dDiameter = prop.get_double(sSurfName + ".diameter")
            od.set(iS, OpticalDevice.DIAMETER, dDiameter)

        if prop.exist(sSurfName + ".diameter.auto"):
            od.set(iS, OpticalDevice.AUTO_DIAMETER, prop.get_bool(sSurfName + ".diameter.auto"))

        # Radius curvature
        if prop.exist(sSurfName + ".radius_curvature"):
            val = prop.get(sSurfName + ".radius_curvature")
            if val == "inf":
                dRadius = 1e10  # infinity
            else:
                dRadius = prop.get_double(sSurfName + ".radius_curvature")
            od.set(iS, OpticalDevice.RADIUS_CURVATURE, dRadius)

        # Conic
        if prop.exist(sSurfName + ".conic"):
            od.set(iS, OpticalDevice.CONIC, prop.get_double(sSurfName + ".conic"))

        # Z/thick
        if prop.exist(sSurfName + ".z"):
            od.set(iS, OpticalDevice.Z, prop.get_double(sSurfName + ".z"))

        if prop.exist(sSurfName + ".thick"):
            od.set(iS, OpticalDevice.THICK, prop.get_double(sSurfName + ".thick"))

        # Material type
        if prop.exist(sSurfName + ".type") and sType not in ["reflect", "image", "stop"]:
            # This would be glass type, but we don't have glass manager yet
            pass

        iS += 1

    # Light settings
    if prop.exist("light.half_field_of_view"):
        od.set_half_field_of_view(prop.get_double("light.half_field_of_view"))

    if prop.exist("light.nbsteps"):
        od.set_nb_angles(prop.get_int("light.nbsteps"))
    else:
        od.set_nb_angles(3)

    if prop.exist("light.colors"):
        od.set_light_colors(prop.get("light.colors"))

    # Autofocus/autocurvature
    if prop.exist("image.autofocus"):
        od.set_autofocus(prop.get_bool("image.autofocus"))

    if prop.exist("image.autocurvature"):
        od.set_image_autocurvature(prop.get_bool("image.autocurvature"))

    return od