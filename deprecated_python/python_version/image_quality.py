class ImageQuality:
    def __init__(self):
        self.vdAngles = []
        self.vdDist = []
        self.vdVignetting = []
        self.spot_size = []
        self.vdSpotvsAiry = []
        self.dFNumber = 0.0
        self.dAirySize = 0.0
        self.isImageInfinite = False
        self._iNbAngles = 0

    def init(self, iNbAngles):
        self._iNbAngles = iNbAngles
        self.vdAngles = [0.0] * iNbAngles
        self.vdDist = [0.0] * iNbAngles
        self.vdVignetting = [0.0] * iNbAngles
        self.spot_size = [0.0] * iNbAngles
        self.vdSpotvsAiry = [0.0] * iNbAngles

    def nb_angles(self):
        return self._iNbAngles