class Photon:
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.z = 0.0
        self.dx = 0.0
        self.dy = 0.0
        self.dz = 0.0
        self.anglex = 0.0
        self.angley = 0.0
        self.valid = True
        self._wavelength = 0.0

    def is_valid(self):
        return self.valid

    def set_wavelength(self, dWavelength):
        self._wavelength = dWavelength

    def wavelength(self):
        return self._wavelength