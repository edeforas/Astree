# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

class ImageQuality:
    def __init__(self):
        self.init(0)

    def init(self, number_of_angles):
        self._number_of_angles = number_of_angles
        self.angle_values = [-1.0] * number_of_angles
        self.distance_values = [-1.0] * number_of_angles
        self.vignetting_values = [-1.0] * number_of_angles
        self.spot_size_values = [-1.0] * number_of_angles
        self.spot_vs_airy_values = [-1.0] * number_of_angles

        self.f_number = -1.0
        self.airy_size = -1.0
        self.is_image_infinite = False

    def nb_angles(self):
        return self._number_of_angles