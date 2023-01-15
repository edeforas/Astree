# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import Glass

class MaterialWater(Glass):
    def __init__(self):
        self.name = "Water"
        self.formula = "Constant"
        self.set_maker("internal")
        self.solid_color = 0xf0f0f0

    def __copy__(self):
        return MaterialWater()

    def clone(self):
        return self.__copy__()

    def compute_index(self, wavelength_microns):
        return 1.333