import math
from photon import Photon
from glass import Glass

class Light:
    def __init__(self):
        self._vPhotons = []
        self._pFirstMaterial = None
        self._pMaterial = None
        self._bMustInit = True
        self._dTiltX = 0.0
        self._dTiltY = 0.0
        self._iDimX = 0
        self._iDimY = 0
        self._iNbPhotons = 0
        self._iNbPhotonsInitVignetting = 0
        self._dDiameter = 0.0
        self._dZ = 0.0
        self._dCenterX = 0.0
        self._dCenterY = 0.0
        self._dSpotSize = 0.0
        self._dFD = 1e99
        self._dVignetting = -1.0
        self._bIsInfinite = False
        self._bIsValid = False
        self._sAllColors = ""

    def set_nb_photons(self, iNbX, iNbY):
        self._iDimX = iNbX
        self._iDimY = iNbY
        self._bMustInit = True

    def nb_photon(self):
        self.init()
        return self._iNbPhotons

    def get_photon(self, iPos):
        self.init()
        return self._vPhotons[iPos]

    def set_tilt(self, dTiltX, dTiltY):
        self._dTiltX = dTiltX * math.pi / 180.0
        self._dTiltY = dTiltY * math.pi / 180.0
        self._bMustInit = True

    def get_tilt(self, dTiltX, dTiltY):
        dTiltX[0] = self._dTiltX * 180.0 / math.pi
        dTiltY[0] = self._dTiltY * 180.0 / math.pi

    def material(self):
        self.init()
        return self._pMaterial

    def set_material(self, pM):
        self._pMaterial = pM

    def set_colors(self, sAllColors):
        self._sAllColors = sAllColors
        if not self._sAllColors:
            self._sAllColors = "YellowBlack."
        self._bMustInit = True

    def set_geometry(self, dZ, dDiameter):
        self._dZ = dZ
        self._dDiameter = dDiameter
        self._bMustInit = True

    def init(self):
        if not self._bMustInit:
            return

        # Simplified: only yellow light
        vdWavelength = [587.5618e-3]  # Yellow

        self._iNbPhotons = self._iDimX * self._iDimY * len(vdWavelength)
        self._vPhotons = [Photon() for _ in range(self._iNbPhotons)]

        iPhoton = 0
        dRadius = self._dDiameter
        dTanX = -math.tan(self._dTiltX)
        dTanY = -math.tan(self._dTiltY)
        dTranslateX = dTanX * dRadius
        dTranslateY = dTanY * dRadius

        for dWavelength in vdWavelength:
            iNMx = self._iDimX // 2
            iNMy = self._iDimY // 2

            for j in range(self._iDimY):
                dPosY = (j - iNMy) * self._dDiameter / iNMy / 2.0 if iNMy > 0 else 0.0
                if self._iDimY == 1:
                    dPosY = 0.0

                for i in range(self._iDimX):
                    p = self._vPhotons[iPhoton]
                    p.set_wavelength(dWavelength)

                    dPosX = (i - iNMx) * self._dDiameter / iNMx / 2.0 if iNMx > 0 else 0.0

                    p.x = dPosX - dTranslateX
                    p.y = dPosY - dTranslateY
                    p.z = self._dZ - dRadius
                    p.dx = dTanX
                    p.dy = dTanY
                    p.dz = 1.0

                    p.valid = True
                    iPhoton += 1

        # Create air material
        self._pFirstMaterial = Glass()
        self._pFirstMaterial.set_name("Air")
        self._pMaterial = self._pFirstMaterial
        self._bIsInfinite = False
        self._bIsValid = True
        self._bMustInit = False

    def compute_spot_size(self, bInfinite=False):
        self._bIsInfinite = bInfinite

        dXS = 0.0
        dYS = 0.0
        dMinX = float('inf')
        dMaxX = float('-inf')
        dMinY = float('inf')
        dMaxY = float('-inf')
        dMinXR = float('inf')
        dMaxXR = float('-inf')
        dMinYR = float('inf')
        dMaxYR = float('-inf')

        dDxCentral = 0.0
        dDyCentral = 0.0
        dDzCentral = 0.0

        iNbValidPhoton = 0

        for p in self._vPhotons:
            if not p.is_valid():
                continue

            if bInfinite:
                px = math.atan2(p.dx, p.dz) * 180.0 / math.pi
                py = math.atan2(p.dy, p.dz) * 180.0 / math.pi
                p.anglex = px
                p.angley = py
            else:
                px = p.x
                py = p.y

            if iNbValidPhoton == 0:
                dMinX = px
                dMaxX = px
                dMinY = py
                dMaxY = py
                dMinXR = px - py
                dMaxXR = dMinXR
                dMinYR = px + py
                dMaxYR = dMinYR
            else:
                dMinX = min(dMinX, px)
                dMaxX = max(dMaxX, px)
                dMinY = min(dMinY, py)
                dMaxY = max(dMaxY, py)
                dMinXR = min(dMinXR, px - py)
                dMaxXR = max(dMaxXR, px - py)
                dMinYR = min(dMinYR, px + py)
                dMaxYR = max(dMaxYR, px + py)

            dXS += px
            dYS += py
            if not bInfinite:
                dDxCentral += p.dx
                dDyCentral += p.dy
                dDzCentral += p.dz
            iNbValidPhoton += 1

        if iNbValidPhoton == 0:
            self._bIsValid = False
            self._dCenterX = 0.0
            self._dCenterY = 0.0
            self._dSpotSize = 1e99
            self._dFD = 1e99
            return

        if not bInfinite:
            # Normalize central direction
            length = math.sqrt(dDxCentral**2 + dDyCentral**2 + dDzCentral**2)
            if length > 0:
                dDxCentral /= length
                dDyCentral /= length
                dDzCentral /= length

            dMinCos = 3.0
            for p in self._vPhotons:
                if not p.is_valid():
                    continue
                dCosP = p.dx * dDxCentral + p.dy * dDyCentral + p.dz * dDzCentral
                if dCosP < 0.0:
                    self._bIsValid = False
                    self._dCenterX = 0.0
                    self._dCenterY = 0.0
                    self._dSpotSize = 1e99
                    self._dFD = 1e99
                    return
                dMinCos = min(dMinCos, dCosP)

            if dMinCos > 1.0:
                dMinCos = 1.0
            elif dMinCos < 0.0:
                dMinCos = 0.0

            theta = math.acos(dMinCos)
            tan_theta = math.tan(theta)
            if tan_theta > 0:
                self._dFD = 0.5 / tan_theta
            else:
                self._dFD = 1e99

            if self._dFD < 0.01:
                self._bIsValid = False
                self._dCenterX = 0.0
                self._dCenterY = 0.0
                self._dSpotSize = 1e99
                self._dFD = 1e99
                return

        self._dCenterX = dXS / iNbValidPhoton
        self._dCenterY = dYS / iNbValidPhoton

        self._dSpotSize = max(dMaxX - dMinX, dMaxY - dMinY)
        dSpotSizeRotated = max(dMaxXR - dMinXR, dMaxYR - dMinYR) * 0.7071067811865475
        self._dSpotSize = max(self._dSpotSize, dSpotSizeRotated)

        self._dVignetting = 100.0 * iNbValidPhoton / self._iNbPhotonsInitVignetting if self._iNbPhotonsInitVignetting > 0 else 0.0
        self._bIsValid = True

    def get_spot_center(self, dCenterX, dCenterY):
        dCenterX[0] = self._dCenterX
        dCenterY[0] = self._dCenterY

    def spot_size(self):
        return self._dSpotSize

    def get_FD(self):
        return self._dFD

    def is_image_infinite(self):
        return self._bIsInfinite

    def airy_radius(self):
        if self.is_image_infinite():
            return 0.02  # in deg
        else:
            return 1.22 * self.get_FD() * 587.5618e-3 * 1e-3

    def spot_vs_airy(self):
        return self.spot_size() / 2.0 / self.airy_radius()

    def init_vignetting(self):
        self._iNbPhotonsInitVignetting = sum(1 for p in self._vPhotons if p.is_valid())

    def vignetting(self):
        return self._dVignetting

    def is_valid(self):
        return self._bIsValid