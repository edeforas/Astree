# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import math

class Photon:
    def __init__(self):
        self.position_x = 0.0
        self.position_y = 0.0
        self.position_z = 0.0

        self.direction_x = 0.0
        self.direction_y = 0.0
        self.direction_z = 0.0

        self.angle_x = 0.0
        self.angle_y = 0.0

        self.wavelength = 0.0

        self.is_valid_photon = False

    def set_wavelength(self, wavelength_value):
        assert wavelength_value > 0.0
        self.wavelength = wavelength_value

    def get_wavelength(self):
        assert self.wavelength > 0.0
        return self.wavelength

    def is_valid(self):
        assert not math.isnan(self.position_x)
        assert not math.isnan(self.position_y)
        assert not math.isnan(self.position_z)
        assert not math.isnan(self.direction_x)
        assert not math.isnan(self.direction_y)
        assert not math.isnan(self.direction_z)

        assert not math.isinf(self.position_x)
        assert not math.isinf(self.position_y)
        assert not math.isinf(self.position_z)
        assert not math.isinf(self.direction_x)
        assert not math.isinf(self.direction_y)
        assert not math.isinf(self.direction_z)

        return self.is_valid_photon