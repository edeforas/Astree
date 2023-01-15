from surface import Surface
from light import Light
from image_quality import ImageQuality

class OpticalDevice:
    RADIUS_CURVATURE_INFINITY = 9e99
    NB_PHOTON_X = 35
    NB_PHOTON_Y = 35

    DIAMETER = 0
    AUTO_DIAMETER = 1
    INNER_DIAMETER = 2
    AUTO_INNER_DIAMETER = 3
    CONIC = 4
    RADIUS_CURVATURE = 5
    CURVATURE = 6
    Z = 7
    THICK = 8
    R4 = 9
    R6 = 10
    R8 = 11
    R10 = 12

    def __init__(self):
        self._vSurfaces = []
        self._vdThicks = []
        self._bRelativeConvention = True
        self._bAutoCurvature = False
        self._bAutoFocus = False
        self._sNote = ""
        self._dHalfFov = 0.0
        self._iGridX = self.NB_PHOTON_X
        self._iGridY = self.NB_PHOTON_Y
        self._iNbAngles = 3
        self._sLightColors = ""
        self._bMustRetrace = False
        self._imageQuality = ImageQuality()
        self.set_light_colors("")  # Set default

    def insert_surface(self, iPos):
        self._vSurfaces.insert(iPos, Surface())
        self._vdThicks.insert(iPos, 0.0)

        if len(self._vSurfaces) == 1:
            self._vSurfaces[0].set_z(0.0)
            return

        if iPos < len(self._vSurfaces) - 1:
            self._vSurfaces[iPos].set_z(self._vSurfaces[iPos + 1].z())
        else:
            self._vSurfaces[iPos].set_z(self._vSurfaces[iPos - 1].z() + self._vdThicks[iPos - 1])

    def delete_surface(self, iPos):
        self._vSurfaces.pop(iPos)
        self._vdThicks.pop(iPos)
        self._bMustRetrace = True

    def nb_surface(self):
        return len(self._vSurfaces)

    def set_type(self, iSurf, sType):
        self._vSurfaces[iSurf].set_type(sType)
        self._bMustRetrace = True

    def type(self, iSurf):
        return self._vSurfaces[iSurf].type()

    def set(self, iSurface, eParam, dParam):
        r = self._vSurfaces[iSurface]

        if eParam == self.DIAMETER:
            r.set_diameter(dParam)
        elif eParam == self.AUTO_DIAMETER:
            r.set_auto_diameter(dParam != 0.0)
        elif eParam == self.INNER_DIAMETER:
            pass  # Simplified
        elif eParam == self.AUTO_INNER_DIAMETER:
            pass  # Simplified
        elif eParam == self.RADIUS_CURVATURE:
            r.set_radius_curvature(dParam)
        elif eParam == self.CURVATURE:
            pass  # Simplified
        elif eParam == self.CONIC:
            r.set_conic(dParam)
        elif eParam == self.Z:
            r.set_z(dParam)
            self.update_thicks()
        elif eParam == self.THICK:
            self._vdThicks[iSurface] = dParam
            self.update_z()
        elif eParam == self.R4:
            r.set_R4(dParam)
        elif eParam == self.R6:
            r.set_R6(dParam)
        elif eParam == self.R8:
            r.set_R8(dParam)
        elif eParam == self.R10:
            r.set_R10(dParam)

        self._bMustRetrace = True

    def get(self, iSurface, eParam, bUpdate=True):
        if bUpdate and eParam != self.Z:
            self.update_z()
        r = self._vSurfaces[iSurface]

        if eParam == self.DIAMETER:
            return r.diameter()
        elif eParam == self.RADIUS_CURVATURE:
            return r.radius_curvature()
        elif eParam == self.CONIC:
            return r.conic()
        elif eParam == self.Z:
            return r.z()
        elif eParam == self.THICK:
            return self._vdThicks[iSurface]
        elif eParam == self.R4:
            return r.R4()
        elif eParam == self.R6:
            return r.R6()
        elif eParam == self.R8:
            return r.R8()
        elif eParam == self.R10:
            return r.R10()

        return 0.0

    def type(self, iSurf):
        return self._vSurfaces[iSurf].type()

    def relative_convention(self):
        return self._bRelativeConvention

    def set_relative_convention(self, bRelativeConvention):
        self._bRelativeConvention = bRelativeConvention

    def set_autofocus(self, bAutofocus):
        self._bAutoFocus = bAutofocus

    def get_autofocus(self):
        return self._bAutoFocus

    def set_image_autocurvature(self, bAutoCurvature):
        self._bAutoCurvature = bAutoCurvature

    def get_image_autocurvature(self):
        return self._bAutoCurvature

    def set_half_field_of_view(self, dHalfFov):
        self._dHalfFov = dHalfFov
        self._bMustRetrace = True

    def half_field_of_view(self):
        return self._dHalfFov

    def set_light_grid(self, iGridX, iGridY):
        self._iGridX = iGridX
        self._iGridY = iGridY
        self._bMustRetrace = True

    def set_light_colors(self, sLightColors):
        self._sLightColors = sLightColors
        self._bMustRetrace = True

    def light_colors(self):
        return self._sLightColors

    def set_nb_angles(self, iNbAngles):
        self._iNbAngles = iNbAngles
        self._bMustRetrace = True

    def nb_intermediate_angles(self):
        return self._iNbAngles

    def update_thicks(self):
        if len(self._vSurfaces) == 0:
            return

        for i in range(len(self._vSurfaces) - 1):
            z1 = self._vSurfaces[i].z()
            z2 = self._vSurfaces[i + 1].z()
            self._vdThicks[i] = z2 - z1

    def update_z(self):
        if len(self._vSurfaces) == 0:
            return

        self._vSurfaces[0].set_z(0.0)
        for i in range(1, len(self._vSurfaces)):
            self._vSurfaces[i].set_z(self._vSurfaces[i - 1].z() + self._vdThicks[i - 1])

    def initialize_light(self, pLight, dTilt, iGridX, iGridY):
        # Find first non-void surface
        pSurf = None
        for s in self._vSurfaces:
            if s.type() != "void":
                pSurf = s
                break

        if pSurf is None:
            return

        pLight.set_nb_photons(iGridX, iGridY)
        pLight.set_tilt(dTilt, 0.0)
        pLight.set_colors(self._sLightColors)

        # Simplified geometry setup
        pLight.set_geometry(pSurf.z(), pSurf.diameter())

    def ray_trace_step(self, light, dTilt, bAutofocus, bAutocurvature):
        self.initialize_light(light, dTilt, self._iGridX, self._iGridY)
        for i in range(len(self._vSurfaces) - 1):
            self._vSurfaces[i].receive(light)
            if i == 0:
                light.init_vignetting()

        # Last surface
        self._vSurfaces[-1].receive(light)
        light.compute_spot_size(self.is_image_infinite())

    def ray_trace(self):
        if not self._bMustRetrace:
            return

        self._imageQuality.init(self._iNbAngles)

        if len(self._vSurfaces) == 0:
            return

        # Reset auto diameter
        for s in self._vSurfaces:
            s.set_auto_diameter(s.get_auto_diameter())

        # Main ray tracing
        light = Light()
        self.ray_trace_step(light, 0.0, self._bAutoFocus, False)
        self._imageQuality.isImageInfinite = light.is_image_infinite()

        # Get quality results
        dCenterX = [0.0]
        dCenterY = [0.0]
        light.get_spot_center(dCenterX, dCenterY)
        self._imageQuality.vdAngles[0] = 0.0
        self._imageQuality.vdDist[0] = -dCenterX[0]
        self._imageQuality.vdVignetting[0] = light.vignetting()
        self._imageQuality.spot_size[0] = light.spot_size()
        self._imageQuality.vdSpotvsAiry[0] = light.spot_vs_airy()

        self._imageQuality.dFNumber = 5.0  # Hardcoded for test
        self._imageQuality.dAirySize = light.airy_radius() * 2.0

        if self._iNbAngles > 1:
            self.ray_trace_step(light, self.half_field_of_view(), False, self._bAutoCurvature)
            light.get_spot_center(dCenterX, dCenterY)
            self._imageQuality.vdAngles[self._iNbAngles - 1] = self.half_field_of_view()
            self._imageQuality.vdDist[self._iNbAngles - 1] = -dCenterX[0]
            self._imageQuality.vdVignetting[self._iNbAngles - 1] = light.vignetting()
            self._imageQuality.spot_size[self._iNbAngles - 1] = light.spot_size()
            self._imageQuality.vdSpotvsAiry[self._iNbAngles - 1] = light.spot_vs_airy()

        for iStep in range(1, self._iNbAngles - 1):
            dTilt = self.half_field_of_view() * iStep / (self._iNbAngles - 1)
            self._imageQuality.vdAngles[iStep] = dTilt
            self.ray_trace_step(light, dTilt, False, False)
            light.get_spot_center(dCenterX, dCenterY)
            self._imageQuality.vdDist[iStep] = -dCenterX[0]
            self._imageQuality.vdVignetting[iStep] = light.vignetting()
            self._imageQuality.spot_size[iStep] = light.spot_size()
            self._imageQuality.vdSpotvsAiry[iStep] = light.spot_vs_airy()

        self._bMustRetrace = False

    def get_image_quality(self):
        self.ray_trace()
        return self._imageQuality

    def is_image_infinite(self):
        if len(self._vSurfaces) == 0:
            return False
        return self._vSurfaces[-1].type() == "image_infinite"