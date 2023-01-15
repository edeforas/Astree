# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import Glass

class MaterialAir(Glass):
    def __init__(self):
        self._sName = "Air"
        self._sFormula = "Custom"
        self.set_maker("internal")
        self._iSolidColor = 0xffffff

    def clone(self):
        return MaterialAir()

    def compute_index(self, dLambdaMicrons):
        sigma_squared = 1.0 / (dLambdaMicrons * dLambdaMicrons)  # dLambda in microns
        return 1.0 + 6.4328e-5 + 2.94981e-2 / (146.0 - sigma_squared) + 2.554e-4 / (41.0 - sigma_squared)

        # source : http://olivier.fournet.free.fr/science_et_physique/refraction_air.htm
        #  with 0.03% de CO2 , 101325 Pa , T= 288.15 K