import math

class Glass:
    def __init__(self):
        self._sMaker = "unknow"
        self._sName = "unknow"
        self._sFormula = "unknow"
        self._iSolidColor = 0
        self._dLastLambda = 0.
        self._dLastIndex = -1.

    def __copy__(self):
        new_glass = Glass()
        new_glass._sMaker = self._sMaker
        new_glass._sName = self._sName
        new_glass._sFormula = self._sFormula
        new_glass._iSolidColor = self._iSolidColor
        new_glass._dLastIndex = -1.
        new_glass._dLastLambda = 0.
        return new_glass

    def __del__(self):
        pass

    def index(self, dLambdaMicrons):
        if dLambdaMicrons != self._dLastLambda:
            self._dLastLambda = dLambdaMicrons
            self._dLastIndex = self.compute_index(dLambdaMicrons)
        return self._dLastIndex

    def maker(self):
        return self._sMaker

    def set_maker(self, sMaker):
        self._sMaker = sMaker

    def name(self):
        return self._sName

    def set_name(self, sName):
        self._sName = sName

    def set_formula(self, sFormula):
        self._sFormula = sFormula

    def solid_color(self):
        return self._iSolidColor

    def set_solid_color(self, iSolidColor):
        self._iSolidColor = iSolidColor

    def formula(self):
        return self._sFormula

    def compute_NdVd(self):
        dLambdaD = 587.5618e-3
        dLambdaF = 486.13e-3
        dLambdaC = 656.27e-3

        Nd = self.index(dLambdaD)
        Nf = self.index(dLambdaF)
        Nc = self.index(dLambdaC)

        if Nf != Nc:
            Vd = (Nd - 1.) / (Nf - Nc)
        else:
            Vd = -1.
        return Nd, Vd

    def compute_NeVe(self):
        dLambdae = 546.07e-3
        dLambdaFp = 479.99e-3
        dLambdaCp = 643.85e-3

        Ne = self.index(dLambdae)
        Nfp = self.index(dLambdaFp)
        Ncp = self.index(dLambdaCp)

        if Nfp != Ncp:
            Ve = (Ne - 1.) / (Nfp - Ncp)
        else:
            Ve = -1.
        return Ne, Ve

    def compute_solid_color(self):
        Nd, Vd = self.compute_NdVd()

        dRed90 = 150
        dGreen90 = 255
        dBlue90 = 255

        dRed20 = 30
        dGreen20 = 150
        dBlue20 = 255

        t = (90. - Vd) / (90. - 20.)
        t = max(0., min(1., t))

        red = dRed90 * t + (1. - t) * dRed20
        green = dGreen90 * t + (1. - t) * dGreen20
        blue = dBlue90 * t + (1. - t) * dBlue20
        self._iSolidColor = int((int(red) << 16) + (int(green) << 8) + int(blue))

    def compute_index(self, dLambdaMicrons):
        raise NotImplementedError("Subclasses must implement compute_index")