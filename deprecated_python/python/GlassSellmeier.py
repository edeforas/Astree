# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import math

class GlassSellmeier:
    def __init__(self):
        self._sName = "Sellmeier"
        self._sFormula = "Sellmeier"

        self._dB1 = 0.0
        self._dB2 = 0.0
        self._dB3 = 0.0
        self._dC1 = 0.0
        self._dC2 = 0.0
        self._dC3 = 0.0

    def clone(self):
        return GlassSellmeier()

    def __init__(self, other):
        assert other._sFormula == "Sellmeier"

        self._dB1 = other._dB1
        self._dB2 = other._dB2
        self._dB3 = other._dB3
        self._dC1 = other._dC1
        self._dC2 = other._dC2
        self._dC3 = other._dC3

    def compute_index(self, d_lambda_microns):
        # use the Sellmeier formula
        l2 = d_lambda_microns * d_lambda_microns
        return math.sqrt(1.0 + self._dB1 * l2 / (l2 - self._dC1) +
                         self._dB2 * l2 / (l2 - self._dC2) +
                         self._dB3 * l2 / (l2 - self._dC3))

    def set_coefs(self, dB1, dB2, dB3, dC1, dC2, dC3):
        self._dB1 = dB1
        self._dB2 = dB2
        self._dB3 = dB3
        self._dC1 = dC1
        self._dC2 = dC2
        self._dC3 = dC3

        self.compute_solid_color()