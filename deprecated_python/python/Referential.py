import math

class Referential:
    def __init__(self):
        self._rotMat = [1., 0., 0., 0., 1., 0., 0., 0., 1.]
        self._x = 0.
        self._y = 0.
        self._z = 0.

    def setYawPitchRoll(self, Yaw, Pitch, Roll):
        A = math.cos(Roll)
        B = math.sin(Roll)
        C = math.cos(Pitch)
        D = math.sin(Pitch)
        E = math.cos(Yaw)
        F = math.sin(Yaw)

        self._rotMat[0] = C * E
        self._rotMat[1] = -C * F
        self._rotMat[2] = -D

        self._rotMat[3] = -B * D * E + A * F
        self._rotMat[4] = B * D * F + A * E
        self._rotMat[5] = -B * C

        self._rotMat[6] = A * D * E + B * F
        self._rotMat[7] = -A * D * F + B * E
        self._rotMat[8] = A * C

    def transform(self, x, y, z):
        xt = x + self._x
        yt = y + self._y
        zt = z + self._z

        nx = self._rotMat[0] * xt + self._rotMat[1] * yt + self._rotMat[2] * zt
        ny = self._rotMat[3] * xt + self._rotMat[4] * yt + self._rotMat[5] * zt
        nz = self._rotMat[6] * xt + self._rotMat[7] * yt + self._rotMat[8] * zt

        return nx, ny, nz

    def unTransform(self, x, y, z):
        nx = self._rotMat[0] * x + self._rotMat[3] * y + self._rotMat[6] * z
        ny = self._rotMat[1] * x + self._rotMat[4] * y + self._rotMat[7] * z
        nz = self._rotMat[2] * x + self._rotMat[5] * y + self._rotMat[8] * z

        return nx - self._x, ny - self._y, nz - self._z

    def setTranslation(self, x, y, z):
        self._x = x
        self._y = y
        self._z = z