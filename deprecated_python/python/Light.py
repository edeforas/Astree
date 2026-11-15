import math
from typing import List, Tuple
import Photon
import Glass

PI = 3.1415926535897
RAD_TO_DEG = 360. / 2. / PI
DEG_TO_RAD = PI * 2. / 360.

def RGB(r: int, g: int, b: int) -> int:
    return (r << 16) + (g << 8) + b

RED = RGB(255, 0, 0)
GREEN = RGB(0, 255, 0)
BLUE = RGB(0, 0, 255)
DARK_GREY = RGB(32, 32, 32)
YELLOW = 0xede500
BROWN = 0x74572e
PURPLE = 0x772176

LAMBDA_IR = 1000.e-3
LAMBDA_RED = 656.2725e-3
LAMBDA_YELLOW = 587.5618e-3
LAMBDA_GREEN = 546.0740e-3
LAMBDA_BLUE = 486.1327e-3
LAMBDA_UV = 300.e-3


class Light:
    def __init__(self):
        self._iNbPhotons = 0
        self._dTiltX = 0.
        self._dTiltY = 0.
        self._dSpotSize = 0.
        self._dCenterX = 0.
        self._dCenterY = 0.
        self._dDiameter = 0.
        self._dZ = 0.
        self._dFD = 1.e99
        self._bMustInit = True
        self._bYellowBlack = False
        self._pFirstMaterial = None
        self._pMaterial = self._pFirstMaterial
        self._dVignetting = -1.
        self._iNbPhotonsInitVignetting = 0
        self._bIsInfinite = False
        self._bIsValid = False
        self._iDimX = 0
        self._iDimY = 0
        self._vPhotons = []
        self._sAllColors = ""

    def __del__(self):
        # GlassManager destruction logic would go here
        pass

    def nb_photon(self) -> int:
        self.init()
        return self._iNbPhotons

    def get_photon(self, iPos: int) -> 'Photon':
        self.init()
        assert iPos >= 0
        assert iPos < self._iNbPhotons
        return self._vPhotons[iPos]

    def get_all_photons(self) -> List:
        self.init()
        return self._vPhotons

    def photons(self) -> List:
        return self._vPhotons  # Warning: may be uninitialized

    def set_geometry(self, dZ: float, dDiameter: float) -> None:
        assert dDiameter != -1.
        self._dZ = dZ
        self._dDiameter = dDiameter
        self._bMustInit = True

    def init(self) -> None:
        if not self._bMustInit:
            return

        # Light direction
        dTanX = -math.tan(self._dTiltX)
        dTanY = -math.tan(self._dTiltY)
        dIdentN = +1.

        vdLambda = []
        viVisualColors = []
        self.calc_colors(self._sAllColors, vdLambda, viVisualColors)

        self._iNbPhotons = int(self._iDimX * self._iDimY * len(vdLambda))
        self._vPhotons = [Photon() for _ in range(self._iNbPhotons)]

        iPhoton = 0
        dRadius = self._dDiameter  # TODO
        dTranslateX = dTanX * dRadius
        dTranslateY = dTanY * dRadius

        for iLambda in range(len(vdLambda)):
            dLambda = vdLambda[iLambda]
            iNMx = self._iDimX // 2
            iNMy = self._iDimY // 2

            for j in range(self._iDimY):
                dPosY = (j - iNMy) * self._dDiameter / float(iNMy) / 2.
                if self._iDimY == 1:
                    dPosY = 0

                for i in range(self._iDimX):
                    p = self._vPhotons[iPhoton]
                    p.set_lambda(dLambda)

                    dPosX = (i - iNMx) * self._dDiameter / float(iNMx) / 2.

                    p.x = dPosX - dTranslateX
                    p.y = dPosY - dTranslateY
                    p.z = self._dZ - dRadius
                    p.dx = dTanX
                    p.dy = dTanY
                    p.dz = dIdentN

                    p.valid = True
                    assert p.is_valid()
                    iPhoton += 1

        # GlassManager initialization would go here
        self._pFirstMaterial = None  # Replace with GlassManager.create("Air")
        self._pMaterial = self._pFirstMaterial
        self._bIsInfinite = False
        self._bIsValid = True
        self._bMustInit = False

    def set_tilt(self, dTiltX: float, dTiltY: float) -> None:
        self._dTiltX = dTiltX * DEG_TO_RAD
        self._dTiltY = dTiltY * DEG_TO_RAD
        self._bMustInit = True

    def get_tilt(self) -> Tuple[float, float]:
        return self._dTiltX * RAD_TO_DEG, self._dTiltY * RAD_TO_DEG

    def calc_colors(self, sColors: str, vdLambda: List[float], vdVisualColors: List[int]) -> None:
        vdLambda.clear()
        vdVisualColors.clear()

        if "IR." in sColors:
            vdLambda.append(LAMBDA_IR)
            vdVisualColors.append(BROWN)

        if "Red." in sColors:
            vdLambda.append(LAMBDA_RED)
            vdVisualColors.append(RED)

        if "YellowBlack." in sColors:
            vdLambda.append(LAMBDA_YELLOW)
            vdVisualColors.append(DARK_GREY)

        if "Yellow." in sColors:
            vdLambda.append(LAMBDA_YELLOW)
            vdVisualColors.append(YELLOW)

        if "Green." in sColors:
            vdLambda.append(LAMBDA_GREEN)
            vdVisualColors.append(GREEN)

        if "Blue." in sColors:
            vdLambda.append(LAMBDA_BLUE)
            vdVisualColors.append(BLUE)

        if "UV." in sColors:
            vdLambda.append(LAMBDA_UV)
            vdVisualColors.append(PURPLE)

    def get_visual_color(self, dLambda: float) -> int:
        if dLambda == LAMBDA_IR:
            return BROWN
        elif dLambda == LAMBDA_RED:
            return RED
        elif dLambda == LAMBDA_YELLOW:
            return DARK_GREY if self._bYellowBlack else YELLOW
        elif dLambda == LAMBDA_GREEN:
            return GREEN
        elif dLambda == LAMBDA_BLUE:
            return BLUE
        elif dLambda == LAMBDA_UV:
            return PURPLE
        return 0

    def set_colors(self, sAllColors: str) -> None:
        self._sAllColors = sAllColors

        if not self._sAllColors:
            self._sAllColors = "YellowBlack."

        self._bYellowBlack = (self._sAllColors == "YellowBlack.")
        self._bMustInit = True

    def set_nb_photons(self, iNbX: int, iNbY: int) -> None:
        assert iNbX > 0
        assert iNbY > 0

        self._iDimX = iNbX
        self._iDimY = iNbY
        self._bMustInit = True

    def material(self) -> 'Glass':
        self.init()
        assert self._pMaterial is not None
        return self._pMaterial

    def set_material(self, pM: 'Glass') -> None:
        assert pM is not None
        self._pMaterial = pM

    def get_spot_center(self) -> Tuple[float, float]:
        return self._dCenterX, self._dCenterY

    def spot_size(self) -> float:
        return self._dSpotSize

    def is_image_infinite(self) -> bool:
        return self._bIsInfinite

    def compute_spot_size(self, bInfinite: bool) -> None:
        dXS, dYS = 0., 0.
        dMinX, dMaxX, dMinY, dMaxY = 0., 0., 0., 0.
        dMinXR, dMaxXR, dMinYR, dMaxYR = 0., 0., 0., 0.

        dDxCentral, dDyCentral, dDzCentral = 0., 0., 0.

        self._bIsInfinite = bInfinite

        iNbValidPhoton = 0
        for i in range(self._iNbPhotons):
            p = self._vPhotons[i]

            if not p.is_valid():
                continue

            if bInfinite:
                px = math.atan2(p.dx, p.dz) * 180. / PI
                py = math.atan2(p.dy, p.dz) * 180. / PI

                p.anglex = px
                p.angley = py
            else:
                px = p.x
                py = p.y

            if iNbValidPhoton == 0:
                dMinX = dMaxX = px
                dMinY = dMaxY = py
                dMinXR = dMaxXR = px - py
                dMinYR = dMaxYR = px + py
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
                # Vector3D.normalize would be implemented elsewhere
                norm = math.sqrt(p.dx**2 + p.dy**2 + p.dz**2)
                p.dx /= norm
                p.dy /= norm
                p.dz /= norm
                
                dDxCentral += p.dx
                dDyCentral += p.dy
                dDzCentral += p.dz

            iNbValidPhoton += 1

        if iNbValidPhoton == 0:
            self._bIsValid = False
            self._dCenterX = 0.
            self._dCenterY = 0.
            self._dSpotSize = 1.e99
            self._dFD = 1.e99
            return

        if bInfinite:
            self._dFD = 1.e99
        else:
            # Normalize central direction
            norm = math.sqrt(dDxCentral**2 + dDyCentral**2 + dDzCentral**2)
            dDxCentral /= norm
            dDyCentral /= norm
            dDzCentral /= norm

            dMinCos = 3.
            for i in range(self._iNbPhotons):
                p = self._vPhotons[i]

                if not p.is_valid():
                    continue

                dCosP = p.dx * dDxCentral + p.dy * dDyCentral + p.dz * dDzCentral
                if dCosP < 0.:
                    self._bIsValid = False
                    self._dCenterX = 0.
                    self._dCenterY = 0.
                    self._dSpotSize = 1.e99
                    self._dFD = 1.e99
                    return

                dCosP = max(0., min(1., dCosP))
                dMinCos = min(dMinCos, dCosP)

            assert 0. <= dMinCos <= 1.

            if dMinCos > 1:
                self._bIsValid = False
                self._dCenterX = 0.
                self._dCenterY = 0.
                self._dSpotSize = 1.e99
                self._dFD = 1.e99
                return

            self._dFD = 0.5 / math.tan(math.acos(dMinCos))

            if self._dFD < 0.01:
                self._bIsValid = False
                self._dCenterX = 0.
                self._dCenterY = 0.
                self._dSpotSize = 1.e99
                self._dFD = 1.e99
                return

        self._dCenterX = dXS / iNbValidPhoton
        self._dCenterY = dYS / iNbValidPhoton

        self._dSpotSize = max(dMaxX - dMinX, dMaxY - dMinY)
        dSpotSizeRotated = max(dMaxXR - dMinXR, dMaxYR - dMinYR) * 0.7071067811865475
        self._dSpotSize = max(self._dSpotSize, dSpotSizeRotated)

        self._dVignetting = 100. * float(iNbValidPhoton) / self._iNbPhotonsInitVignetting
        self._bIsValid = True

    def get_FD(self) -> float:
        return self._dFD

    def spot_vs_airy(self) -> float:
        return self.spot_size() / 2. / self.airy_radius()

    def airy_radius(self) -> float:
        if self.is_image_infinite():
            return 0.02  # in deg
        else:
            return 1.22 * self.get_FD() * LAMBDA_YELLOW * 1.e-3

    def vignetting(self) -> float:
        return self._dVignetting

    def init_vignetting(self) -> None:
        self._iNbPhotonsInitVignetting = 0
        for i in range(self._iNbPhotons):
            if self._vPhotons[i].is_valid():
                self._iNbPhotonsInitVignetting += 1

    def is_valid(self) -> bool:
        return self._bIsValid