class Glass:
    def __init__(self):
        self._sMaker = ""
        self._sName = ""
        self._sFormula = ""
        self._iSolidColor = 0

    def clone(self):
        return Glass()

    def index(self, dWavelengthMicrons):
        return self.compute_index(dWavelengthMicrons)

    def solid_color(self):
        return self._iSolidColor

    def set_solid_color(self, iSolidColor):
        self._iSolidColor = iSolidColor

    def maker(self):
        return self._sMaker

    def set_maker(self, sMaker):
        self._sMaker = sMaker

    def name(self):
        return self._sName

    def set_name(self, sName):
        self._sName = sName

    def formula(self):
        return self._sFormula

    def set_formula(self, sFormula):
        self._sFormula = sFormula

    def compute_index(self, dWavelengthMicrons):
        return 1.0  # Default to air

    def compute_NdVd(self, Nd, Vd):
        # Simplified implementation
        pass

    def compute_NeVe(self, Ne, Ve):
        # Simplified implementation
        pass