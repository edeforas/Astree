# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

from Light import Light
from Photon import Photon

SPOT_SIZE_INFINITY = 1.e99

class LightAutofocus:
    def __init__(self):
        self._x_center = 0.
        self._y_center = 0.

    def autofocus(self, light):
        dB = 0.
        dA = dB - 1000.
        dC = dB + 1000.
        dZ_step = 1.e-9

        # find a point that is valid for initial start
        i_scale = 0
        dQA, dQC, dQB = self.compute_spot_size(light, dB)
        while (i_scale < 11) and (dQB >= SPOT_SIZE_INFINITY / 2):  # 2km max
            dA = dB - 2 * (dB - dA)  # look at left
            dQA = self.compute_spot_size(light, dA)

            if dQA < SPOT_SIZE_INFINITY / 2:
                dB = dA
                dQB = dQA
                break

            dC = dB - 2 * (dB - dC)  # look at right
            dQC = self.compute_spot_size(light, dC)
            if dQC < SPOT_SIZE_INFINITY / 2:
                dB = dC
                dQB = dQC
                break
            i_scale += 1

        if dQB >= SPOT_SIZE_INFINITY / 2:
            return 0.  # unable to find a valid start point

        # find valid dA and dB and such as dA >= dB and dC >= dB
        dA = dB - 1000.
        dC = dB + 1000.
        dQA = self.compute_spot_size(light, dA)
        dQC = self.compute_spot_size(light, dC)

        # 1st part: expand A and C, so B became a minimum
        i_scale = 0
        while (i_scale < 11) and (dQA <= dQB):  # 2km max
            dA = dB - 2 * (dB - dA)
            dQA = self.compute_spot_size(light, dA)
            i_scale += 1

        i_scale = 0
        while (i_scale < 11) and (dQC <= dQB):  # 2km max
            dC = dB - 2 * (dB - dC)
            dQC = self.compute_spot_size(light, dC)
            i_scale += 1

        # exit if no solution
        if (dQA <= dQB) or (dQC <= dQB):
            return 0.

        # reduce interval around solution using dichotomy 1.5
        while dC - dA > dZ_step:
            dAB = (dA + dB) * 0.5
            dQAB = self.compute_spot_size(light, dAB)

            if dQAB < dQB:
                # a new minimum
                dC = dB
                dB = dAB
                dQB = dQAB
            else:
                dBC = (dB + dC) * 0.5
                dQBC = self.compute_spot_size(light, dBC)

                if dQBC < dQB:
                    # a new minimum
                    dA = dB
                    dB = dBC
                    dQB = dQBC
                else:
                    # minimum is B
                    dA = dAB
                    dC = dBC

        # the minimum is at dB;

        # TODO check all photon are ok at dB, else return false; or use return value of compute_size
        # TODO use golden search

        return dB

    def compute_spot_size(self, light, z):
        # compute the intersection of all valid photon with plane at (0,0,z) with normal (1,0,0)

        # TODO add 45deg (or more) calliper

        # TODO add early abort tests on previous spot size
        x_max = -1.e99
        x_min = 1.e99

        y_max = -1.e99
        y_min = 1.e99
        one_found = False

        for photon in light.photons():
            if not photon.is_valid():
                continue

            if photon.dz == 0.:
                continue  # TODO check clean error

            # compute t at z
            t = (z - photon.z) / photon.dz

            if t <= 0.:
                return SPOT_SIZE_INFINITY  # all photons must be used in autofocus, for now

            x = photon.x + t * photon.dx
            y = photon.y + t * photon.dy

            if not one_found:
                x_max = x_min = x
                y_max = y_min = y

                one_found = True
                continue

            x_max = max(x_max, x)
            x_min = min(x_min, x)

            y_max = max(y_max, y)
            y_min = min(y_min, y)

        if not one_found:
            return SPOT_SIZE_INFINITY

        self._x_center = (x_max + x_min) * 0.5
        self._y_center = (y_max + y_min) * 0.5

        if x_max - x_min > y_max - y_min:
            return x_max - x_min
        else:
            return y_max - y_min

    def get_center(self):
        return self._x_center, self._y_center