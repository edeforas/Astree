# This file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# Please see LICENSE.txt for more details and licensing issues
# Copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com

import math
from enum import Enum

eFullFrameMaxError = 1
eCenterOnly = 2
eFullFrameMean = 3
eMostlyCenter = 4


eInvalidState=1
eBetterSolutionFound=2
eNoBetterSolution=3
eSolutionOnEdge=4
eNothingToOptimize=5


SPOT_SIZE_INFINITY = 1e20


class DeviceOptimizerParameter:
    def __init__(self):
        self.iSurface = 0
        self.sParameter = ""
        self.dMin = 0.
        self.dMax = 0.
        self.dValue = 0.
        self.dResolution = 0.

class DeviceOptimizer:
    # Constants (these should be defined somewhere in your actual implementation)
    CONIC_HALF_RANGE_REFINE = 1.0
    RCURV_HALF_RATIO_REFINE = 1.1
    R4_HALF_RATIO_REFINE = 1.1
    R6_HALF_RATIO_REFINE = 1.1
    R8_HALF_RATIO_REFINE = 1.1
    R10_HALF_RATIO_REFINE = 1.1
    Z_HALF_RANGE_REFINE = 1.0
    THICK_HALF_RANGE_REFINE = 1.0
    
    CONIC_HALF_RANGE = 10.0
    RCURV_HALF_RATIO = 2.0
    R4_HALF_RANGE_DEFAULT = 1e-6
    R6_HALF_RANGE_DEFAULT = 1e-9
    R8_HALF_RANGE_DEFAULT = 1e-12
    R10_HALF_RANGE_DEFAULT = 1e-15
    Z_HALF_RANGE = 10.0
    THICK_HALF_RANGE = 10.0
    
    MIN_RESOLUTION_CONIC = 0.01
    MIN_RESOLUTION_RCURV = 0.001
    MIN_RESOLUTION_CURVATURE = 0.000001
    MIN_RESOLUTION_THICK = 0.001
    MIN_RESOLUTION_R4 = 1e-8
    MIN_RESOLUTION_R6 = 1e-11
    MIN_RESOLUTION_R8 = 1e-14
    MIN_RESOLUTION_R10 = 1e-17
    

    def __init__(self):
        self._pDevice = None
        self._meritFunction = DeviceOptimizer.eFullFrameMaxError
        self._dMinVignetting = 100.0
        self._parameters = []

    def clear(self):
        self._pDevice = None
        self._meritFunction = DeviceOptimizer.eFullFrameMaxError
        self._parameters.clear()

    def set_device(self, pDevice):
        self._pDevice = pDevice

    def set_min_vignetting(self, dMinVignetting):
        self._dMinVignetting = dMinVignetting

    def add_parameter(self, iSurface, sParameter, bRefine):
        dMin = 0.0
        dMax = 0.0

        assert self._pDevice is not None

        if bRefine:
            if sParameter == "Conic":
                dMin = self._pDevice.get(iSurface, "CONIC") - self.CONIC_HALF_RANGE_REFINE
                dMax = self._pDevice.get(iSurface, "CONIC") + self.CONIC_HALF_RANGE_REFINE

            if sParameter == "RCurv":
                dMin = self._pDevice.get(iSurface, "RADIUS_CURVATURE") / self.RCURV_HALF_RATIO_REFINE
                dMax = self._pDevice.get(iSurface, "RADIUS_CURVATURE") * self.RCURV_HALF_RATIO_REFINE

            if sParameter == "R4":
                dR4 = self._pDevice.get(iSurface, "R4")
                if dR4 != 0.0:
                    dMin = dR4 / self.R4_HALF_RATIO_REFINE
                    dMax = dR4 * self.R4_HALF_RATIO_REFINE
                else:
                    dMin = -self.R4_HALF_RANGE_DEFAULT
                    dMax = self.R4_HALF_RANGE_DEFAULT

            if sParameter == "R6":
                dR6 = self._pDevice.get(iSurface, "R6")
                if dR6 != 0.0:
                    dMin = dR6 / self.R6_HALF_RATIO_REFINE
                    dMax = dR6 * self.R6_HALF_RATIO_REFINE
                else:
                    dMin = -self.R6_HALF_RANGE_DEFAULT
                    dMax = self.R6_HALF_RANGE_DEFAULT

            if sParameter == "R8":
                dR8 = self._pDevice.get(iSurface, "R8")
                if dR8 != 0.0:
                    dMin = dR8 / self.R8_HALF_RATIO_REFINE
                    dMax = dR8 * self.R8_HALF_RATIO_REFINE
                else:
                    dMin = -self.R8_HALF_RANGE_DEFAULT
                    dMax = self.R8_HALF_RANGE_DEFAULT

            if sParameter == "R10":
                dR10 = self._pDevice.get(iSurface, "R10")
                if dR10 != 0.0:
                    dMin = dR10 / self.R10_HALF_RATIO_REFINE
                    dMax = dR10 * self.R10_HALF_RATIO_REFINE
                else:
                    dMin = -self.R10_HALF_RANGE_DEFAULT
                    dMax = self.R10_HALF_RANGE_DEFAULT

            if sParameter == "Z":
                dMin = self._pDevice.get(iSurface, "Z") - self.Z_HALF_RANGE_REFINE
                dMax = self._pDevice.get(iSurface, "Z") * self.Z_HALF_RANGE_REFINE

            if sParameter == "Thick":
                dMin = self._pDevice.get(iSurface, "THICK") - self.THICK_HALF_RANGE_REFINE
                dMax = self._pDevice.get(iSurface, "THICK") + self.THICK_HALF_RANGE_REFINE
        else:
            if sParameter == "Conic":
                dMin = self._pDevice.get(iSurface, "CONIC") - self.CONIC_HALF_RANGE
                dMax = self._pDevice.get(iSurface, "CONIC") + self.CONIC_HALF_RANGE

            if sParameter == "RCurv":
                dMin = self._pDevice.get(iSurface, "RADIUS_CURVATURE") / self.RCURV_HALF_RATIO
                dMax = self._pDevice.get(iSurface, "RADIUS_CURVATURE") * self.RCURV_HALF_RATIO

            if sParameter == "R4":
                dR4 = self._pDevice.get(iSurface, "R4")
                if dR4 != 0.0:
                    dMin = dR4 / self.R4_HALF_RATIO
                    dMax = dR4 * self.R4_HALF_RATIO
                else:
                    dMin = -self.R4_HALF_RANGE_DEFAULT
                    dMax = self.R4_HALF_RANGE_DEFAULT

            if sParameter == "R6":
                dR6 = self._pDevice.get(iSurface, "R6")
                if dR6 != 0.0:
                    dMin = dR6 / self.R6_HALF_RATIO
                    dMax = dR6 * self.R6_HALF_RATIO
                else:
                    dMin = -self.R6_HALF_RANGE_DEFAULT
                    dMax = self.R6_HALF_RANGE_DEFAULT

            if sParameter == "R8":
                dR8 = self._pDevice.get(iSurface, "R8")
                if dR8 != 0.0:
                    dMin = dR8 / self.R8_HALF_RATIO
                    dMax = dR8 * self.R8_HALF_RATIO
                else:
                    dMin = -self.R8_HALF_RANGE_DEFAULT
                    dMax = self.R8_HALF_RANGE_DEFAULT

            if sParameter == "R10":
                dR10 = self._pDevice.get(iSurface, "R10")
                if dR10 != 0.0:
                    dMin = dR10 / self.R10_HALF_RATIO
                    dMax = dR10 * self.R10_HALF_RATIO
                else:
                    dMin = -self.R10_HALF_RANGE_DEFAULT
                    dMax = self.R10_HALF_RANGE_DEFAULT

            if sParameter == "Z":
                dMin = self._pDevice.get(iSurface, "Z") - self.Z_HALF_RANGE
                dMax = self._pDevice.get(iSurface, "Z") * self.Z_HALF_RANGE

            if sParameter == "Thick":
                dMin = self._pDevice.get(iSurface, "THICK") - self.THICK_HALF_RANGE
                dMax = self._pDevice.get(iSurface, "THICK") + self.THICK_HALF_RANGE

        self.add_parameter_with_limits(iSurface, sParameter, dMin, dMax)

    def add_parameter_with_limits(self, iSurface, sParameter, dMin, dMax):
        dop = DeviceOptimizerParameter()
        
        if sParameter == "RCurv":  # always optimise with Curvature instead of RCurv
            dop.sParameter = "Curvature"
            dMin = 1.0 / dMin
            dMax = 1.0 / dMax
        else:
            dop.sParameter = sParameter

        dop.iSurface = iSurface

        if dMin < dMax:
            dop.dMin = dMin
            dop.dMax = dMax
        else:
            dop.dMin = dMax
            dop.dMax = dMin

        dop.dValue = (dMin + dMax) / 2.0

        if sParameter == "Conic":
            dop.dResolution = self.MIN_RESOLUTION_CONIC

        if sParameter == "RCurv":
            dop.dResolution = self.MIN_RESOLUTION_RCURV

        if sParameter == "Curvature":
            dop.dResolution = self.MIN_RESOLUTION_CURVATURE

        if sParameter == "thick":
            dop.dResolution = self.MIN_RESOLUTION_THICK

        if sParameter == "Z":
            dop.dResolution = self.MIN_RESOLUTION_THICK

        if sParameter == "R4":
            dop.dResolution = self.MIN_RESOLUTION_R4

        if sParameter == "R6":
            dop.dResolution = self.MIN_RESOLUTION_R6

        if sParameter == "R8":
            dop.dResolution = self.MIN_RESOLUTION_R8

        if sParameter == "R10":
            dop.dResolution = self.MIN_RESOLUTION_R10

        self._parameters.append(dop)

    def set_merit_function(self, eMeritFunction):
        self._meritFunction = eMeritFunction

    def apply_parameter(self, parameters):
        assert self._pDevice is not None

        for param in parameters:
            sParam = param.sParameter
            iSurface = param.iSurface
            dVal = param.dValue

            if sParam == "Conic":
                self._pDevice.set(iSurface, "CONIC", dVal)

            if sParam == "RCurv":
                self._pDevice.set(iSurface, "RADIUS_CURVATURE", dVal)

            if sParam == "Curvature":
                self._pDevice.set(iSurface, "CURVATURE", dVal)

            if sParam == "Z":
                self._pDevice.set(iSurface, "Z", dVal)

            if sParam == "Thick":
                self._pDevice.set(iSurface, "THICK", dVal)

            if sParam == "R4":
                self._pDevice.set(iSurface, "R4", dVal)

            if sParam == "R6":
                self._pDevice.set(iSurface, "R6", dVal)

            if sParam == "R8":
                self._pDevice.set(iSurface, "R8", dVal)

            if sParam == "R10":
                self._pDevice.set(iSurface, "R10", dVal)

    def compute_demerit(self):
        pQ = self._pDevice.get_image_quality()

        for i in range(pQ.nb_angles()):
            if math.isnan(pQ.vdSpotSize[i]):
                return self.SPOT_SIZE_INFINITY

            if math.isnan(pQ.vdSpotvsAiry[i]):
                return self.SPOT_SIZE_INFINITY

            if pQ.vdVignetting[i] < self._dMinVignetting:
                return self.SPOT_SIZE_INFINITY

        if math.isnan(pQ.dAirySize):
            return self.SPOT_SIZE_INFINITY

        if self._meritFunction == DeviceOptimizer.eCenterOnly:
            return pQ.vdSpotSize[0]

        if self._meritFunction == DeviceOptimizer.eFullFrameMean:
            dMeritMoy = 0.0
            for i in range(pQ.nb_angles()):
                dMeritMoy += pQ.vdSpotSize[i]
            return dMeritMoy

        if self._meritFunction == DeviceOptimizer.eMostlyCenter:
            dCentralWeight = 3.0
            dMeritMoy = 0.0
            for i in range(pQ.nb_angles()):
                dFactor = 1.0 + (pQ.nb_angles() - 1.0 - i) / float(pQ.nb_angles() - 1.0) * (dCentralWeight - 1.0)
                dMeritMoy += pQ.vdSpotSize[i] * dFactor
            return dMeritMoy

        if self._meritFunction == DeviceOptimizer.eFullFrameMaxError:
            dMeritMoy = 0.0
            for i in range(pQ.nb_angles()):
                if dMeritMoy < pQ.vdSpotSize[i]:
                    dMeritMoy = pQ.vdSpotSize[i]
            return dMeritMoy

        return self.SPOT_SIZE_INFINITY
