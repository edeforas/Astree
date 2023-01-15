import math
from typing import List, Optional
import Light

import ImageQuality
import LightAutofocus
import Surface

class OpticalDevice:
    NB_PHOTON_X = 0  # Default value, adjust as needed
    NB_PHOTON_Y = 0  # Default value, adjust as needed

    def __init__(self):
        self._iGridX = self.NB_PHOTON_X
        self._iGridY = self.NB_PHOTON_Y
        self._iNbAngles = 3
        self._dHalfFov = 0.0
        self._bMustRetrace = False
        self._bAutoCurvature = False
        self._bAutoFocus = False
        self._bRelativeConvention = True
        self._sNote = ""
        self._vSurfaces = []
        self._vdThicks = []
        self._surfParamsClone = []
        self._imageQuality = ImageQuality.ImageQuality()
        self._lightColors = []  # Assuming this stores color information

    def __copy__(self):
        new_device = OpticalDevice()
        new_device._vdThicks = self._vdThicks.copy()
        new_device._vSurfaces = [surface.copy() for surface in self._vSurfaces]
        new_device.set_relative_convention(self.relative_convention())
        new_device.set_note(self.note())
        new_device.set_half_field_of_view(self.half_field_of_view())
        new_device._iGridX = self._iGridX
        new_device._iGridY = self._iGridY
        new_device.set_nb_intermediate_angles(self.nb_intermediate_angles())
        new_device.set_light_colors(self.light_colors().copy())
        new_device._bAutoCurvature = self._bAutoCurvature
        new_device._bAutoFocus = self._bAutoFocus
        new_device._bMustRetrace = True
        return new_device

    def copy(self):
        return self.__copy__()

    def insert_surface(self, iPos: int):
        assert iPos >= 0
        assert iPos <= self.nb_surface()

        self._vSurfaces.insert(iPos, Surface.Surface())
        self._vdThicks.insert(iPos, 0.0)

        if self.nb_surface() == 1:  # first time
            self._vSurfaces[0].set_z(0)
            return

        if iPos < self.nb_surface() - 1:
            # intermediate position
            self._vSurfaces[iPos].set_z(self._vSurfaces[iPos + 1].z())
        else:
            # last position
            self._vSurfaces[iPos].set_z(self._vSurfaces[iPos - 1].z() + self._vdThicks[iPos - 1])

        # update clone
        for i in range(len(self._surfParamsClone)):
            # if clone gap increased, increment ref
            if self._surfParamsClone[i].iRefSurface >= iPos:
                self._surfParamsClone[i].iRefSurface += 1
            if self._surfParamsClone[i].iSurface >= iPos:
                self._surfParamsClone[i].iSurface += 1

    def delete_surface(self, iPos: int):
        assert iPos >= 0
        assert iPos < len(self._vSurfaces)

        del self._vdThicks[iPos]
        del self._vSurfaces[iPos]

        self.update_thicks()
        self._bMustRetrace = True

        # update clone
        i = 0
        while i < len(self._surfParamsClone):
            if (self._surfParamsClone[i].iSurface == iPos or 
                self._surfParamsClone[i].iRefSurface == iPos):  # link broken, remove
                del self._surfParamsClone[i]
            else:
                # if clone removed, decrement ref or orig
                if self._surfParamsClone[i].iRefSurface > iPos:
                    self._surfParamsClone[i].iRefSurface -= 1
                if self._surfParamsClone[i].iSurface > iPos:
                    self._surfParamsClone[i].iSurface -= 1
                i += 1

    def set_relative_convention(self, bRelativeConvention: bool):
        self._bRelativeConvention = bRelativeConvention

    def relative_convention(self) -> bool:
        return self._bRelativeConvention

    def update_thicks(self):
        if self.nb_surface() == 0:
            return

        for i in range(self.nb_surface() - 1):
            z1 = self._vSurfaces[i].z()
            z2 = self._vSurfaces[i + 1].z()
            self._vdThicks[i] = z2 - z1

    def update_z(self):
        if self.nb_surface() == 0:
            return

        dZ = self._vSurfaces[0].z()

        for i in range(self.nb_surface()):
            self._vSurfaces[i].set_z(dZ)
            dZ += self._vdThicks[i]

    def set_note(self, sNote: str):
        self._sNote = sNote

    def note(self) -> str:
        return self._sNote

    def has_aspheric(self) -> bool:
        for i in range(self.nb_surface()):
            if self._vSurfaces[i].is_aspheric():
                return True
        return False

    def has_inner_diameter(self) -> bool:
        for i in range(self.nb_surface()):
            if self._vSurfaces[i].inner_diameter() != 0.0:
                return True
        return False

    def has_auto(self) -> bool:
        if self._bAutoFocus:
            return True
        if self._bAutoCurvature:
            return True

        for i in range(self.nb_surface()):
            s = self._vSurfaces[i]
            bAuto = s.get_auto_diameter() or s.get_auto_inner_diameter()
            if bAuto:
                return True
        return False

    def set_autofocus(self, bAutofocus: bool):
        self._bMustRetrace = (not self._bAutoFocus) and bAutofocus
        self._bAutoFocus = bAutofocus

    def get_autofocus(self) -> bool:
        return self._bAutoFocus

    def set_image_autocurvature(self, bAutoCurvature: bool):
        self._bMustRetrace = (not self._bAutoCurvature) and bAutoCurvature
        self._bAutoCurvature = bAutoCurvature

    def get_image_autocurvature(self) -> bool:
        return self._bAutoCurvature

    def set_comment(self, iSurface: int, sComment: str):
        self._vSurfaces[iSurface].set_comment(sComment)

    def comment(self, iSurface: int) -> str:
        return self._vSurfaces[iSurface].comment()

    def has_comment(self) -> bool:
        for i in range(self.nb_surface()):
            if not self._vSurfaces[i].comment():
                return True
        return False

    def nb_surface(self) -> int:
        return len(self._vSurfaces)

    def ray_trace(self):
        if not self._bMustRetrace:
            return

        self._imageQuality.init(self._iNbAngles)

        if self.nb_surface() == 0:
            return

        # reset autodiameter
        for i in range(self.nb_surface()):
            self._vSurfaces[i].set_auto_diameter(self._vSurfaces[i].get_auto_diameter())
            self._vSurfaces[i].set_auto_inner_diameter(self._vSurfaces[i].get_auto_inner_diameter())

        # update clone parameters
        for a in self._surfParamsClone:
            self.set(a.iSurface, a.param, a.dGain * self.get(a.iRefSurface, a.param, False))

        # main ray tracing loop on axis for autofocus
        light = Light()
        self.ray_trace_step(light, 0, self._bAutoFocus, False)
        self._imageQuality.isImageInfinite = light.is_image_infinite()

        # get quality result
        dCenterX, dCenterY = light.get_spot_center()
        self._imageQuality.vdAngles[0] = 0
        self._imageQuality.vdDist[0] = -dCenterX
        self._imageQuality.vdVignetting[0] = light.vignetting()
        self._imageQuality.vdSpotSize[0] = light.spot_size()
        self._imageQuality.vdSpotvsAiry[0] = light.spot_vs_airy()

        # for autofocus and on-axis stats
        self._imageQuality.dFNumber = light.get_FD()
        self._imageQuality.dAirySize = light.airy_radius() * 2.0

        if self._iNbAngles > 1:  # for autocurvature
            self.ray_trace_step(light, self.half_field_of_view(), False, self._bAutoCurvature)

            # get quality result
            dCenterX, dCenterY = light.get_spot_center()
            self._imageQuality.vdAngles[self._iNbAngles - 1] = self.half_field_of_view()
            self._imageQuality.vdDist[self._iNbAngles - 1] = -dCenterX
            self._imageQuality.vdVignetting[self._iNbAngles - 1] = light.vignetting()
            self._imageQuality.vdSpotSize[self._iNbAngles - 1] = light.spot_size()
            self._imageQuality.vdSpotvsAiry[self._iNbAngles - 1] = light.spot_vs_airy()
            self._imageQuality.isImageInfinite = self.is_image_infinite()

        for iStep in range(1, self._iNbAngles - 1):
            dTilt = self.half_field_of_view() * iStep / float(self._iNbAngles - 1)
            self._imageQuality.vdAngles[iStep] = dTilt

            self.ray_trace_step(light, dTilt, False, False)

            # get quality result
            dCenterX, dCenterY = light.get_spot_center()
            self._imageQuality.vdDist[iStep] = -dCenterX
            self._imageQuality.vdVignetting[iStep] = light.vignetting()
            self._imageQuality.vdSpotSize[iStep] = light.spot_size()
            self._imageQuality.vdSpotvsAiry[iStep] = light.spot_vs_airy()

        self._bMustRetrace = False

    def ray_trace_step(self, light: Light, dTilt: float, bAutofocus: bool, bAutocurvature: bool):
        self.initialize_light(light, dTilt, self._iGridX, self._iGridY)
        for i in range(self.nb_surface() - 1):
            self._vSurfaces[i].receive(light)
            if i == 0:
                light.init_vignetting()

        # if autofocus, compute z before last receive
        if bAutofocus:
            # compute AF on light
            la = LightAutofocus.LightAutofocus()
            dZ = la.autofocus(light)
            self._vSurfaces[self.nb_surface() - 1].set_z(dZ)
            self.update_thicks()

        if bAutocurvature:
            # compute AF on light
            la = LightAutofocus.LightAutofocus()
            dZ = la.autofocus(light)

            dCenterX, dCenterY = la.get_center()

            # set new curvature
            dX1 = dCenterX
            dZ1 = self._vSurfaces[self.nb_surface() - 1].z() - dZ
            dR = -0.5 * (dX1 * dX1 + dZ1 * dZ1) / dZ1  # R =0.5*(x1*x1+z1*z1)/z1
            # TODO test dR
            self._vSurfaces[self.nb_surface() - 1].set_radius_curvature(dR)

        # finish receiving
        s = self._vSurfaces[self.nb_surface() - 1]
        s.receive(light)

        light.compute_spot_size(self.is_image_infinite())

    def set_light_grid(self, iGridX: int, iGridY: int):
        if iGridX == self._iGridX and iGridY == self._iGridY:
            return

        self._iGridX = iGridX
        self._iGridY = iGridY
        self._bMustRetrace = True

    def compute_light(self, pLight: Light, iSurface: int, dTilt: float, iGridX: int, iGridY: int):
        self.ray_trace()  # for auto diameter and autofocus
        self.initialize_light(pLight, dTilt, iGridX, iGridY)

        for i in range(iSurface + 1):
            self._vSurfaces[i].receive(pLight)
            if i == 0:
                pLight.init_vignetting()

        # finish computing quality
        pLight.compute_spot_size(self.is_image_infinite())

    # Additional methods that were referenced but not defined in the original code
    def set_half_field_of_view(self, value: float):
        self._dHalfFov = value

    def half_field_of_view(self) -> float:
        return self._dHalfFov

    def set_nb_intermediate_angles(self, value: int):
        self._iNbAngles = value

    def nb_intermediate_angles(self) -> int:
        return self._iNbAngles

    def set_light_colors(self, colors):
        self._lightColors = colors

    def light_colors(self):
        return self._lightColors

    def is_image_infinite(self) -> bool:
        # This method was referenced but not defined in the original
        return False  # Placeholder implementation

    def initialize_light(self, light: Light, dTilt: float, iGridX: int, iGridY: int):
        # This method was referenced but not defined in the original
        pass

    def set(self, iSurface: int, param: str, value: float):
        # This method was referenced but not defined in the original
        pass

    def get(self, iSurface: int, param: str, bCheckClone: bool) -> float:
        # This method was referenced but not defined in the original
        return 0.0  # Placeholder implementation

    def set_type(self, iSurf, sType):
        self._vSurfaces[iSurf].set_type(sType)
        self._bMustRetrace = True

    def type(self, iSurf):
        return self._vSurfaces[iSurf].type()

    def get_image_quality(self):
        self.ray_trace()
        return self._imageQuality  # TODO modify ref instead of copy

    def nb_intermediate_angles(self):
        return self._iNbAngles

    def set_nb_intermediate_angles(self, iNbAngles):
        if iNbAngles == self._iNbAngles:
            return

        if iNbAngles < 1:
            iNbAngles = 1

        self._iNbAngles = iNbAngles
        self._bMustRetrace = True

    def get(self, iSurface, eParam, bUpdate):
        if bUpdate:
            self.ray_trace()

        r = self._vSurfaces[iSurface]

        if eParam == 'CONIC':
            return r.conic()
        if eParam == 'R4':
            return r.R4()
        if eParam == 'R6':
            return r.R6()
        if eParam == 'R8':
            return r.R8()
        if eParam == 'R10':
            return r.R10()
        if eParam == 'AUTO_DIAMETER':
            return r.get_auto_diameter()
        if eParam == 'AUTO_INNER_DIAMETER':
            return r.get_auto_inner_diameter()
        if eParam == 'Z':
            return r.z()
        if eParam == 'THICK':
            return self._vdThicks[iSurface]
        if eParam == 'RADIUS_CURVATURE':
            return r.radius_curvature()
        if eParam == 'CURVATURE':
            return r.curvature()
        if eParam == 'DIAMETER':
            return r.diameter()
        if eParam == 'INNER_DIAMETER':
            return r.inner_diameter()

        return -1

    def set(self, iSurface, eParam, dParam):
        r = self._vSurfaces[iSurface]

        if eParam == 'DIAMETER':
            r.set_diameter(dParam)
        if eParam == 'AUTO_DIAMETER':
            r.set_auto_diameter(dParam != 0.)
        if eParam == 'INNER_DIAMETER':
            r.set_inner_diameter(dParam)
        if eParam == 'AUTO_INNER_DIAMETER':
            r.set_auto_inner_diameter(dParam != 0.)
        if eParam == 'RADIUS_CURVATURE':
            r.set_radius_curvature(dParam)
        if eParam == 'CURVATURE':
            r.set_curvature(dParam)
        if eParam == 'CONIC':
            r.set_conic(dParam)
        if eParam == 'Z':
            r.set_z(dParam)
            self.update_thicks()
        if eParam == 'THICK':
            self._vdThicks[iSurface] = dParam
            self.update_z()
        if eParam == 'R4':
            r.set_R4(dParam)
        if eParam == 'R6':
            r.set_R6(dParam)
        if eParam == 'R8':
            r.set_R8(dParam)
        if eParam == 'R10':
            r.set_R10(dParam)

        self._bMustRetrace = True

    def set_parameter(self, sKey, sValue):
        self._otherParameters[sKey] = sValue

    def set_parameter_double(self, sKey, dValue):
        self._otherParameters[sKey] = str(dValue)

    def get_parameter(self, sKey):
        if sKey not in self._otherParameters:
            return False, None

        dValue = float(self._otherParameters[sKey])
        return True, dValue  # TODO test conversion result
        
    def get_parameter(self, sKey):
        if sKey not in self._otherParameters:
            return False, None
        return True, self._otherParameters[sKey]

    def all_parameters(self):
        return self._otherParameters

    def set_clone(self, iSurface, eParam, iRefSurface, dGain):
        eParamAlias = eParam

        # Z and THICK are same
        if eParam == 'THICK':
            eParamAlias = 'Z'
        if eParam == 'Z':
            eParamAlias = 'THICK'

        if iRefSurface >= self.nb_surface():
            return

        if iRefSurface >= iSurface:  # only backward cloning allowed (avoid loop)
            return

        for i in range(len(self._surfParamsClone)):
            if (self._surfParamsClone[i]['iSurface'] == iSurface and
                    (self._surfParamsClone[i]['param'] == eParam or
                     self._surfParamsClone[i]['param'] == eParamAlias)):
                if iRefSurface >= 0:
                    # already existing, updating
                    self._surfParamsClone[i]['iRefSurface'] = iRefSurface
                    self._surfParamsClone[i]['dGain'] = dGain
                else:
                    # remove clone
                    del self._surfParamsClone[i]
                self._bMustRetrace = True
                return

        if iRefSurface < 0:  # nothing to remove
            return

        # create new entry
        cloneSurf = {
            'iSurface': iSurface,
            'param': eParam,
            'iRefSurface': iRefSurface,
            'dGain': dGain
        }

        self._surfParamsClone.append(cloneSurf)
        self._bMustRetrace = True

    def get_clone(self, iSurface, eParam):
        eParamAlias = eParam

        # Z and THICK are same
        if eParam == 'THICK':
            eParamAlias = 'Z'
        if eParam == 'Z':
            eParamAlias = 'THICK'

        for i in range(len(self._surfParamsClone)):
            if (self._surfParamsClone[i]['iSurface'] == iSurface and
                    (self._surfParamsClone[i]['param'] == eParam or
                     self._surfParamsClone[i]['param'] == eParamAlias)):
                return True, self._surfParamsClone[i]['iRefSurface'], self._surfParamsClone[i]['dGain']

        return False, None, None

    def is_clone(self, iSurface, eParam):
        return self.get_clone(iSurface, eParam)[0]

    def is_image_infinite(self):
        for surface in self._vSurfaces:
            if surface.type() == "image_infinite":
                return True
        return False

    def nb_surface(self):
        return len(self._vSurfaces)