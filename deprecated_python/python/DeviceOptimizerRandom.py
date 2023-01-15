import random
import OpticalDevice
import DeviceOptimizer

RANDOM_ITER_BY_AXIS=10

class DeviceOptimizerRandom(DeviceOptimizer):
    def __init__(self):
        super().__init__()

    def __del__(self):
        pass

    def optimize(self):
        assert self._pDevice is not None

        dMeritOrig = self.compute_demerit()
        deviceOrig = OpticalDevice(self._pDevice)

        if not self._parameters:
            return DeviceOptimizer.eNothingToOptimize

        paramBest = self._parameters

        iNbIter = 1
        for _ in range(len(paramBest)):
            iNbIter *= RANDOM_ITER_BY_AXIS

        dBestMerit = DeviceOptimizer.SPOT_SIZE_INFINITY

        bExit = False
        while not bExit:
            paramBestIter = paramBest
            for _ in range(iNbIter):
                paramBestTest = paramBest
                for param in paramBestTest:
                    param.dValue = param.dMin + (param.dMax - param.dMin) * random.random()
                    assert param.dMin <= param.dValue <= param.dMax

                self.apply_parameter(paramBestTest)
                dMerit = self.compute_demerit()

                if dMerit < dBestMerit:
                    paramBestIter = paramBestTest
                    dBestMerit = dMerit

            paramBest = paramBestIter

            bExit = True
            for param in paramBest:
                dCenter = param.dValue
                dRadius = (param.dMax - param.dMin) / 4.

                param.dMin = dCenter - dRadius
                param.dMax = dCenter + dRadius

                if param.dMax - param.dMin > param.dResolution:
                    bExit = False

        if dBestMerit < DeviceOptimizer.SPOT_SIZE_INFINITY / 2:
            if dBestMerit < dMeritOrig:
                self._parameters = paramBest
                self.apply_parameter(paramBest)
                return DeviceOptimizer.eBetterSolutionFound

        self._pDevice = deviceOrig
        return DeviceOptimizer.eNoBetterSolution