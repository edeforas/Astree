import numpy as np
import DeviceOptimizer
import OpticalDevice

DIVIDE_ITER_BY_AXIS=5

class DeviceOptimizerHypercube(DeviceOptimizer):
    def __init__(self):
        super().__init__()

    def __del__(self):
        pass

    def optimize(self):
        # divide hypercube into regular domain
        assert self._pDevice is not None

        dMeritOrig = self.compute_demerit()
        deviceOrig = OpticalDevice(self._pDevice)

        if not self._parameters:
            return DeviceOptimizer.eNothingToOptimize

        paramBest = self._parameters.copy()

        # compute number of iterations
        iNbIter = 1
        for _ in paramBest:
            iNbIter *= DIVIDE_ITER_BY_AXIS

        dBestMerit = DeviceOptimizer.SPOT_SIZE_INFINITY  # TODO init with actual solution

        bExit = False
        while not bExit:
            paramBestIter = paramBest.copy()
            vDivide = [0] * len(paramBest)

            for i in range(iNbIter):
                paramBestTest = paramBest.copy()
                for iP in range(len(paramBestTest)):
                    dop = paramBestTest[iP]

                    dop.dValue = dop.dMin + (dop.dMax - dop.dMin) * vDivide[iP] / DIVIDE_ITER_BY_AXIS
                    assert dop.dValue <= dop.dMax
                    assert dop.dValue >= dop.dMin

                self.apply_parameter(paramBestTest)
                dMerit = self.compute_demerit()

                if dMerit < dBestMerit:
                    paramBestIter = paramBestTest
                    dBestMerit = dMerit

                # change next parameter in vDivide
                for j in range(len(vDivide)):
                    if vDivide[j] != DIVIDE_ITER_BY_AXIS:
                        vDivide[j] += 1
                        break
                    else:
                        vDivide[j] = 0  # and jump to next parameter

            paramBest = paramBestIter

            # scale around best solution, divide by 2 each dimension test if box is too small then exit
            bExit = True
            for iP in range(len(paramBest)):
                dop = paramBest[iP]
                dCenter = dop.dValue
                dRadius = (dop.dMax - dop.dMin) / 4.0

                dop.dMin = dCenter - dRadius
                dop.dMax = dCenter + dRadius

                # check domain exit
                if (dop.dMax - dop.dMin) > dop.dResolution:
                    bExit = False

        if dBestMerit < DeviceOptimizer.SPOT_SIZE_INFINITY / 2:
            if dBestMerit < dMeritOrig:
                self._parameters = paramBest
                self.apply_parameter(paramBest)
                return DeviceOptimizer.eBetterSolutionFound

        # restore device original settings
        self._pDevice = deviceOrig
        return DeviceOptimizer.eNoBetterSolution