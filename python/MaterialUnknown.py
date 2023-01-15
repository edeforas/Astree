# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import Glass

class MaterialUnknown(Glass):
    def __init__(self):
        self.name = "Unknown"
        self.formula = "Unknown"
        self.solid_color = 0xf0f0f0

    def __copy__(self):
        return MaterialUnknown()

    def compute_index(self, lambda_microns):
        return 1.0