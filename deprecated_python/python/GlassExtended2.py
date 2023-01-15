# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import math

# glass definition as in the https://github.com/nzhagen/zemaxglass

class GlassExtended2:
    def __init__(self):
        self.name = "Extended2"
        self.formula = "Extended2"

        self.coefficient_0 = 0.0
        self.coefficient_1 = 0.0
        self.coefficient_2 = 0.0
        self.coefficient_3 = 0.0
        self.coefficient_4 = 0.0
        self.coefficient_5 = 0.0
        self.coefficient_6 = 0.0
        self.coefficient_7 = 0.0

    def clone(self):
        return GlassExtended2(self)

    def __init__(self, other):
        assert other.formula == "Extended2"

        self.coefficient_0 = other.coefficient_0
        self.coefficient_1 = other.coefficient_1
        self.coefficient_2 = other.coefficient_2
        self.coefficient_3 = other.coefficient_3
        self.coefficient_4 = other.coefficient_4
        self.coefficient_5 = other.coefficient_5
        self.coefficient_6 = other.coefficient_6
        self.coefficient_7 = other.coefficient_7

    def compute_index(self, lambda_microns):
        # use the Extended2 formulae
        # n^2 = c0 + c1 λ^2 + c2 λ^−2 + c3 λ^−4 + c4 λ^−6 + c5 λ^−8 + c6 λ^4 + c7 λ^6
        
        lambda_squared = lambda_microns * lambda_microns
        lambda_fourth = lambda_squared * lambda_squared
        inverse_lambda_squared = 1.0 / lambda_squared
        inverse_lambda_fourth = inverse_lambda_squared * inverse_lambda_squared
        
        # todo optimize with the Horner rule
        n_squared = (self.coefficient_0 + self.coefficient_1 * lambda_squared +
                      self.coefficient_2 * inverse_lambda_squared +
                      self.coefficient_3 * inverse_lambda_fourth +
                      self.coefficient_4 * inverse_lambda_squared * inverse_lambda_fourth +
                      self.coefficient_5 * inverse_lambda_fourth * inverse_lambda_fourth +
                      self.coefficient_6 * lambda_fourth +
                      self.coefficient_7 * lambda_fourth * lambda_squared)
        return math.sqrt(n_squared)

    def set_coefficients(self, coefficient_0, coefficient_1, coefficient_2, coefficient_3, coefficient_4, coefficient_5, coefficient_6, coefficient_7):
        self.coefficient_0 = coefficient_0
        self.coefficient_1 = coefficient_1
        self.coefficient_2 = coefficient_2
        self.coefficient_3 = coefficient_3
        self.coefficient_4 = coefficient_4
        self.coefficient_5 = coefficient_5
        self.coefficient_6 = coefficient_6
        self.coefficient_7 = coefficient_7

        self.compute_solid_color()