# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import math

def normalize(x, y, z):
    squared_norm = x * x + y * y + z * z

    if squared_norm != 0.0:
        squared_norm = 1.0 / math.sqrt(squared_norm)
        x *= squared_norm
        y *= squared_norm
        z *= squared_norm
