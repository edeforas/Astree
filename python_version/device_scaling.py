class DeviceScaling:
    def scale(self, pDevice, dRatio, bScaleDiameter=True, bScaleFocal=True):
        if dRatio <= 0.0:
            return False

        bConvention = pDevice.relative_convention()
        pDevice.set_relative_convention(True)

        for iS in range(pDevice.nb_surface()):
            if bScaleDiameter:
                # Assume no clones
                dDiameter = pDevice.get(iS, pDevice.DIAMETER, False)
                pDevice.set(iS, pDevice.DIAMETER, dDiameter * dRatio)

                # Inner diameter not implemented
                pass

            if bScaleFocal:
                # THICK
                dTHICK = pDevice.get(iS, pDevice.THICK, False)
                pDevice.set(iS, pDevice.THICK, dTHICK * dRatio)

                # RADIUS_CURVATURE
                dRadiusCurvature = pDevice.get(iS, pDevice.RADIUS_CURVATURE, False)
                pDevice.set(iS, pDevice.RADIUS_CURVATURE, dRadiusCurvature * dRatio)

                # Polynomial coefficients not implemented
                pass

        pDevice.set_relative_convention(bConvention)
        return True