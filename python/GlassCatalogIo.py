# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import os

import GlassSellmeier
import GlassExtended2

class GlassCatalogIO:
    def load(self, file_path, glass_manager):
        with open(file_path, 'r') as file:
            glass_pending = False

            glass_name = ""
            glass_formula = 0
            glass_maker = ""
            coefficient_B1 = 0.0
            coefficient_B2 = 0.0
            coefficient_B3 = 0.0
            coefficient_C1 = 0.0
            coefficient_C2 = 0.0
            coefficient_C3 = 0.0
            coefficient_B4 = 0.0
            coefficient_B5 = 0.0

            index_start_maker = file_path.rfind('\\')
            if index_start_maker != -1:
                glass_maker = file_path[index_start_maker + 1:]
            index_separator = next((i for i, char in enumerate(glass_maker) if char in "_. -"), None)
            if index_separator is not None:
                glass_maker = glass_maker[:index_separator]

            for line in file:
                line = line.lstrip()  # trim first space if any

                if len(line) < 2:
                    continue  # nothing to parse

                key = line[:2]
                value = line[2:].lstrip()  # can be empty

                if key == "CC":
                    # pCatalog.set_comment(value)
                    pass

                if key == "NM":
                    if glass_pending:
                        if glass_formula == 2:  # Sellmeier
                            # create and store the glass
                            glass_sellmeier = GlassSellmeier()
                            glass_sellmeier.set_maker(glass_maker)
                            glass_sellmeier.set_name(glass_name)
                            glass_sellmeier.set_coefs(coefficient_B1, coefficient_B2, coefficient_B3, coefficient_C1, coefficient_C2, coefficient_C3)
                            glass_manager.inject(glass_sellmeier)
                        elif glass_formula == 12:  # Extended2
                            # create and store the glass
                            glass_extended2 = GlassExtended2()
                            glass_extended2.set_maker(glass_maker)
                            glass_extended2.set_name(glass_name)
                            glass_extended2.set_coefs(coefficient_B1, coefficient_C1, coefficient_B2, coefficient_C2, coefficient_B3, coefficient_C3, coefficient_B4, coefficient_B5)
                            glass_manager.inject(glass_extended2)
                        else:
                            # TODO
                            pass

                    # compute subkey
                    values = value.split()
                    glass_name = values[0]
                    glass_formula = int(values[1]) if len(values) > 1 else 0

                    glass_pending = True

                if key == "CD":  # GlassCoef formula
                    coefficients = list(map(float, value.split()))
                    coefficient_B1, coefficient_C1, coefficient_B2, coefficient_C2, coefficient_B3, coefficient_C3, coefficient_B4, coefficient_B5 = coefficients

        return True