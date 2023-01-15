import math

def scale(pDevice, dRatio, bScaleDiameter, bScaleFocal):
    assert pDevice is not None

    if dRatio <= 0.:
        return False

    bConvention = pDevice.relative_convention()
    pDevice.set_relative_convention(True)

    for iS in range(pDevice.nb_surface()):
        if bScaleDiameter:
            if not pDevice.is_clone(iS, 'DIAMETER'):
                dDiameter = pDevice.get(iS, 'DIAMETER', False)
                pDevice.set(iS, 'DIAMETER', dDiameter * dRatio)
            if not pDevice.is_clone(iS, 'INNER_DIAMETER'):
                dInnerDiameter = pDevice.get(iS, 'INNER_DIAMETER', False)
                pDevice.set(iS, 'INNER_DIAMETER', dInnerDiameter * dRatio)

        if bScaleFocal:
            if not pDevice.is_clone(iS, 'THICK'):
                dTHICK = pDevice.get(iS, 'THICK', False)
                pDevice.set(iS, 'THICK', dTHICK * dRatio)
            if not pDevice.is_clone(iS, 'RADIUS_CURVATURE'):
                dRadiusCurvature = pDevice.get(iS, 'RADIUS_CURVATURE', False)
                pDevice.set(iS, 'RADIUS_CURVATURE', dRadiusCurvature * dRatio)

            dRatio2 = dRatio * dRatio
            dRatio3 = dRatio2 * dRatio
            dRatio5 = dRatio2 * dRatio3
            dRatio7 = dRatio2 * dRatio5
            dRatio9 = dRatio2 * dRatio7

            if not pDevice.is_clone(iS, 'R4'):
                dR4 = pDevice.get(iS, 'R4', False)
                pDevice.set(iS, 'R4', dR4 / dRatio3)
            if not pDevice.is_clone(iS, 'R6'):
                dR6 = pDevice.get(iS, 'R6', False)
                pDevice.set(iS, 'R6', dR6 / dRatio5)
            if not pDevice.is_clone(iS, 'R8'):
                dR8 = pDevice.get(iS, 'R8', False)
                pDevice.set(iS, 'R8', dR8 / dRatio7)
            if not pDevice.is_clone(iS, 'R10'):
                dR10 = pDevice.get(iS, 'R10', False)
                pDevice.set(iS, 'R10', dR10 / dRatio9)

    pDevice.set_relative_convention(bConvention)
    return True