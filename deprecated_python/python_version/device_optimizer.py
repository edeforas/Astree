import math
import copy

SPOT_SIZE_INFINITY = 1e10

class DeviceOptimizerParameter:
    def __init__(self, iSurface, sParameter, dMin, dMax, dValue, dResolution):
        self.iSurface = iSurface
        self.sParameter = sParameter
        self.dMin = dMin
        self.dMax = dMax
        self.dValue = dValue
        self.dResolution = dResolution

class DeviceOptimizer:
    def __init__(self):
        self._pDevice = None
        self._parameters = []
        self._meritFunction = "eFullFrameMaxError"
        self._dMinVignetting = 100.0

    def set_device(self, pDevice):
        self._pDevice = pDevice

    def set_min_vignetting(self, dMinVignetting):
        self._dMinVignetting = dMinVignetting

    def add_parameter(self, iSurface, sParameter, dMin=None, dMax=None):
        if dMin is None or dMax is None:
            # Auto min max for conic
            if sParameter == "Conic":
                current = self._pDevice.get(iSurface, self._pDevice.CONIC)
                dMin = current - 4
                dMax = current + 4
        dop = DeviceOptimizerParameter(iSurface, sParameter, dMin, dMax, (dMin + dMax) / 2.0, 1e-7)
        self._parameters.append(dop)

    def set_merit_function(self, meritFunction):
        self._meritFunction = meritFunction

    def apply_parameter(self, parameters):
        for param in parameters:
            if param.sParameter == "Conic":
                self._pDevice.set(param.iSurface, self._pDevice.CONIC, param.dValue)
            elif param.sParameter == "Curvature":
                self._pDevice.set(param.iSurface, self._pDevice.CURVATURE, param.dValue)
            elif param.sParameter == "Z":
                self._pDevice.set(param.iSurface, self._pDevice.Z, param.dValue)
            elif param.sParameter == "Thick":
                self._pDevice.set(param.iSurface, self._pDevice.THICK, param.dValue)
            # Add other parameters as needed

    def compute_demerit(self):
        iq = self._pDevice.get_image_quality()
        for i in range(iq.nb_angles()):
            if math.isnan(iq.vdSpotSize[i]):
                return SPOT_SIZE_INFINITY
            if iq.vdVignetting[i] < self._dMinVignetting:
                return SPOT_SIZE_INFINITY
        if math.isnan(iq.dAirySize):
            return SPOT_SIZE_INFINITY

        if self._meritFunction == "eCenterOnly":
            return iq.vdSpotSize[0]
        elif self._meritFunction == "eFullFrameMaxError":
            return max(iq.vdSpotSize)
        else:
            return max(iq.vdSpotSize)  # default

    def optimize_conic_simple(self, iSurface, steps=20):
        # Simple grid search for conic optimization
        if not self._parameters:
            return None

        param = self._parameters[0]  # assume one parameter
        best_value = param.dValue
        best_merit = float('inf')

        step_size = (param.dMax - param.dMin) / steps
        for i in range(steps + 1):
            value = param.dMin + i * step_size
            self._pDevice.set(param.iSurface, self._pDevice.CONIC, value)
            merit = self.compute_demerit()
            if merit < best_merit:
                best_merit = merit
                best_value = value

        # Set to best
        self._pDevice.set(param.iSurface, self._pDevice.CONIC, best_value)
        return best_value

    def optimize_amoeba(self, max_iter=500):
        # Nelder-Mead simplex optimization
        if not self._pDevice or not self._parameters:
            return "eNothingToOptimize"

        dMeritOrig = self.compute_demerit()
        deviceOrig = copy.deepcopy(self._pDevice)

        # Initialize simplex with the center of the definition domain
        simplex = []
        param_center = copy.deepcopy(self._parameters)
        for i in range(len(self._parameters)):
            param_center[i].dValue = (param_center[i].dMin + param_center[i].dMax) * 0.5
        simplex.append(param_center)

        # Initialize simplex with semi-edge of the definition domain
        for i in range(len(self._parameters)):
            param = copy.deepcopy(param_center)
            param[i].dValue = param[i].dValue + (param[i].dMax - param[i].dMin) / 8.0
            simplex.append(param)

        # Compute merit for each simplex point
        vdDemerit = []
        for param in simplex:
            self.apply_parameter(param)
            vdDemerit.append(self.compute_demerit())

        iIter = 0
        bStopCriteria = False
        while iIter < max_iter and not bStopCriteria:
            # Find best, worst, and second worst
            iBest = 0
            dBestMerit = vdDemerit[0]
            iWorse = 0
            dWorse = vdDemerit[0]

            for i in range(1, len(simplex)):
                if vdDemerit[i] < dBestMerit:
                    iBest = i
                    dBestMerit = vdDemerit[i]
                if vdDemerit[i] > dWorse:
                    iWorse = i
                    dWorse = vdDemerit[i]

            # Compute mean of all points except worst
            paramMean = copy.deepcopy(simplex[0])
            for i in range(len(paramMean)):
                paramMean[i].dValue = 0.0

            for i in range(len(simplex)):
                if i != iWorse:
                    for j in range(len(paramMean)):
                        paramMean[j].dValue += simplex[i][j].dValue

            for i in range(len(paramMean)):
                paramMean[i].dValue /= (len(simplex) - 1)

            bFound = False

            # Reflection
            paramMirror = copy.deepcopy(paramMean)
            bOutOfDomain = False
            for i in range(len(paramMirror)):
                paramMirror[i].dValue += (paramMean[i].dValue - simplex[iWorse][i].dValue)
                if paramMirror[i].dValue < paramMirror[i].dMin or paramMirror[i].dValue > paramMirror[i].dMax:
                    bOutOfDomain = True

            dMirror = SPOT_SIZE_INFINITY
            if not bOutOfDomain:
                self.apply_parameter(paramMirror)
                dMirror = self.compute_demerit()

            # Check if reflection is better than second worst but not better than best
            dSecondWorse = max([vd for i, vd in enumerate(vdDemerit) if i != iWorse])
            if dBestMerit <= dMirror < dSecondWorse:
                simplex[iWorse] = paramMirror
                vdDemerit[iWorse] = dMirror
                bFound = True

            # Expansion
            if not bFound and dMirror < dBestMerit:
                paramMirrorFar = copy.deepcopy(paramMean)
                bOutOfDomain2 = False
                for i in range(len(paramMirrorFar)):
                    paramMirrorFar[i].dValue += 2 * (paramMirror[i].dValue - paramMean[i].dValue)
                    if paramMirrorFar[i].dValue < paramMirrorFar[i].dMin or paramMirrorFar[i].dValue > paramMirrorFar[i].dMax:
                        bOutOfDomain2 = True

                dMirrorFar = SPOT_SIZE_INFINITY
                if not bOutOfDomain2:
                    self.apply_parameter(paramMirrorFar)
                    dMirrorFar = self.compute_demerit()

                if dMirrorFar < dMirror:
                    simplex[iWorse] = paramMirrorFar
                    vdDemerit[iWorse] = dMirrorFar
                else:
                    simplex[iWorse] = paramMirror
                    vdDemerit[iWorse] = dMirror
                bFound = True

            # Contraction
            if not bFound:
                paramMirrorContracted = copy.deepcopy(paramMean)
                for i in range(len(paramMirrorContracted)):
                    paramMirrorContracted[i].dValue += 0.5 * (simplex[iWorse][i].dValue - paramMean[i].dValue)

                self.apply_parameter(paramMirrorContracted)
                dContracted = self.compute_demerit()

                if dContracted < dWorse:
                    simplex[iWorse] = paramMirrorContracted
                    vdDemerit[iWorse] = dContracted
                    bFound = True

            # Reduction
            if not bFound:
                for i in range(len(simplex)):
                    if i != iBest:
                        for j in range(len(simplex[i])):
                            simplex[i][j].dValue = simplex[iBest][j].dValue + 0.5 * (simplex[i][j].dValue - simplex[iBest][j].dValue)
                        self.apply_parameter(simplex[i])
                        vdDemerit[i] = self.compute_demerit()

            # Check stop criteria based on simplex size
            vdMinParam = [float('inf')] * len(self._parameters)
            vdMaxParam = [-float('inf')] * len(self._parameters)

            for param in simplex:
                for j in range(len(param)):
                    vdMinParam[j] = min(vdMinParam[j], param[j].dValue)
                    vdMaxParam[j] = max(vdMaxParam[j], param[j].dValue)

            bStopCriteria = True
            for i in range(len(vdMinParam)):
                if vdMaxParam[i] - vdMinParam[i] > self._parameters[i].dResolution:
                    bStopCriteria = False

            iIter += 1

        # Apply best solution if better
        if dBestMerit < SPOT_SIZE_INFINITY / 2 and dBestMerit < dMeritOrig:
            self._parameters = simplex[iBest]
            self.apply_parameter(self._parameters)
            return "eBetterSolutionFound"
        else:
            # Restore original device
            self._pDevice = deviceOrig
            return "eNoBetterSolution"