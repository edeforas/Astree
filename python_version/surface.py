import math
from photon import Photon
from glass import Glass

class Surface:
    def __init__(self):
        self._sType = "void"
        self._x = 0.0
        self._y = 0.0
        self._z = 0.0
        self._dDiameter = 0.0
        self._dInnerDiameter = 0.0
        self._bAutoDiameter = False
        self._bAutoInnerDiameter = False
        self._dConic = 0.0
        self._dCurvature = 0.0
        self._dR4 = 0.0
        self._dR6 = 0.0
        self._dR8 = 0.0
        self._dR10 = 0.0
        self._bIsPerfect = False
        self._sComment = ""
        self._pMaterial = None
        self._pMaterialNext = None
        self._pMaterialPrev = None
        self.update_geometry()

    def update_geometry(self):
        self._bIsFlat = abs(self._dCurvature) < 1e-20
        self._bIsSpherical = self._dConic == 0.0
        self._bIsConic = self._dConic != 0.0
        self._bIsAspheric = self._dR4 != 0.0 or self._dR6 != 0.0 or self._dR8 != 0.0 or self._dR10 != 0.0
        self._bIsPerfect = self._sType in ["perfect_lens", "perfect_mirror"]

    def set_type(self, sType):
        self._sType = sType
        self._bIsPerfect = (sType == "perfect_mirror")

    def type(self):
        return self._sType

    def set_z(self, dz):
        self._z = dz

    def z(self):
        return self._z

    def set_diameter(self, dDiameter):
        self._dDiameter = dDiameter

    def diameter(self):
        return self._dDiameter

    def set_auto_diameter(self, bAutoDiameter):
        self._bAutoDiameter = bAutoDiameter

    def get_auto_diameter(self):
        return self._bAutoDiameter

    def set_radius_curvature(self, dRadiusCurvature):
        if dRadiusCurvature == 0.0:
            self._dCurvature = 0.0
        else:
            self._dCurvature = 1.0 / dRadiusCurvature
        self.update_geometry()

    def radius_curvature(self):
        if self._dCurvature == 0.0:
            return 0.0
        else:
            return 1.0 / self._dCurvature

    def set_conic(self, dConic):
        self._dConic = dConic
        self.update_geometry()

    def conic(self):
        return self._dConic

    def set_R4(self, dR4):
        self._dR4 = dR4
        self.update_geometry()

    def R4(self):
        return self._dR4

    def set_R6(self, dR6):
        self._dR6 = dR6
        self.update_geometry()

    def R6(self):
        return self._dR6

    def set_R8(self, dR8):
        self._dR8 = dR8
        self.update_geometry()

    def R8(self):
        return self._dR8

    def set_R10(self, dR10):
        self._dR10 = dR10
        self.update_geometry()

    def R10(self):
        return self._dR10

    def set_comment(self, sComment):
        self._sComment = sComment

    def comment(self):
        return self._sComment

    def local_ref(self, p):
        p.x -= self._x
        p.y -= self._y
        p.z -= self._z

    def global_ref(self, p):
        p.x += self._x
        p.y += self._y
        p.z += self._z

    def receive(self, light):
        if self._sType == "reflect" or self._sType == "perfect_mirror":
            self.reflect(light)
        elif self._sType == "stop" or self._sType == "image":
            self.stop(light)
        elif self._sType == "void":
            pass
        else:
            self.transmit(light)

    def reflect(self, light):
        for i in range(light.nb_photon()):
            p = light.get_photon(i)
            if not p.is_valid():
                continue
            self.local_ref(p)
            self.reflect_photon(p)
            self.global_ref(p)

    def stop(self, light):
        for i in range(light.nb_photon()):
            p = light.get_photon(i)
            if not p.is_valid():
                continue
            self.local_ref(p)
            self.stop_photon(p)
            self.global_ref(p)

    def transmit(self, light):
        """
        Process transmission of light through surface.
        Updates material tracking and calls transmit_photon for each photon.
        Matches Surface.cpp implementation.
        """
        self._pMaterialPrev = light.material()
        self._pMaterialNext = self._pMaterial

        for i in range(light.nb_photon()):
            p = light.get_photon(i)

            if not p.is_valid():
                continue

            self.local_ref(p)
            self.transmit_photon(p)
            self.global_ref(p)

        # Update light material if surface has material and is not perfect
        if self._pMaterial is not None and not self._bIsPerfect:
            light.set_material(self._pMaterial)

    def transmit_photon(self, p):
        """
        Compute transmission (refraction) of photon through surface.
        Uses Snell's law with refractive indices.
        Matches Surface.cpp implementation.
        """
        if not p.is_valid():
            return

        self.stop_photon(p)
        if not p.is_valid():
            return

        if self._bIsPerfect:
            # Perfect lens: compute focal point transmission
            if p.dz == 0.0:
                p.valid = False
                return

            # Flat lens case
            if self._bIsFlat:
                return

            # Curved lens: transmit through focal point
            dFocal = 0.5 / self._dCurvature
            t = dFocal / p.dz

            if p.dz < 0.0:
                t = -t

            ax = p.dx * t
            ay = p.dy * t
            az = p.dz * t

            p.dx = ax - p.x
            p.dy = ay - p.y
            p.dz = az - p.z

            if dFocal < 0.0:
                p.dx = -p.dx
                p.dy = -p.dy
                p.dz = -p.dz

            return

        # General refraction using Snell's law
        if self._pMaterialNext is None or self._pMaterialPrev is None:
            p.valid = False
            return

        nx, ny, nz = self.compute_normal(p.x, p.y, p.z)

        # Normalize normal vector
        dRadiusSq = p.x * p.x + p.y * p.y
        if dRadiusSq > 1e-20:
            # Use normalized normal
            norm = math.sqrt(nx*nx + ny*ny + nz*nz)
            if norm > 0:
                nx /= norm
                ny /= norm
                nz /= norm
            else:
                nx = 0.0
                ny = 0.0
                nz = 1.0
        else:
            # At surface center or flat surface
            nx = 0.0
            ny = 0.0
            nz = 1.0

        u = nx * p.dx + ny * p.dy + nz * p.dz

        # Compute refractive index ratio
        dC12 = u * u / (p.dx*p.dx + p.dy*p.dy + p.dz*p.dz)
        dRatioN = self._pMaterialNext.index(p.wavelength()) / self._pMaterialPrev.index(p.wavelength())
        denom = dRatioN*dRatioN + dC12 - 1.0

        if denom <= 0.0:
            # Total internal reflection
            p.valid = False
            return

        dT2T1 = math.sqrt(dC12 / denom)
        k = (1.0 - dT2T1) * u

        p.dx = nx * k + dT2T1 * p.dx
        p.dy = ny * k + dT2T1 * p.dy
        p.dz = nz * k + dT2T1 * p.dz

    def reflect_photon(self, p):
        """
        Compute reflection of photon at surface.
        Matches Surface.cpp implementation.
        """
        if not p.is_valid():
            return

        self.stop_photon(p)
        if not p.is_valid():
            return

        if self._bIsPerfect:
            # Perfect mirror: compute focal point reflection
            if p.dz == 0.0:
                p.valid = False
                return

            # Flat mirror case
            if self._bIsFlat:
                p.dz = -p.dz
                return

            # Curved mirror: reflect to focal point
            dFocal = 0.5 / self._dCurvature
            t = dFocal / p.dz
            ax = p.dx * t
            ay = p.dy * t
            az = p.dz * t

            if self._dCurvature > 0.0:
                if p.dz > 0.0:
                    p.dx = ax + p.x
                    p.dy = ay + p.y
                    p.dz = -az - p.z
                else:
                    p.dx = -ax - p.x
                    p.dy = -ay - p.y
                    p.dz = az - p.z
            else:
                if p.dz > 0.0:
                    p.dx = -ax - p.x
                    p.dy = -ay - p.y
                    p.dz = az - p.z
                else:
                    p.dx = ax + p.x
                    p.dy = ay + p.y
                    p.dz = -az - p.z
            return

        # General surface reflection using normal vector
        nx, ny, nz = self.compute_normal(p.x, p.y, p.z)

        # Normalize and compute reflection: r = d - 2(d·n)n
        dRayonSq = nx*nx + ny*ny + nz*nz
        if dRayonSq > 0:
            u = 2.0 * (nx * p.dx + ny * p.dy + nz * p.dz) / dRayonSq
            p.dx -= nx * u
            p.dy -= ny * u
            p.dz -= nz * u

    def compute_normal(self, x, y, z):
        """Compute surface normal at point (x, y, z) - matches Surface.cpp implementation"""
        # For flat surface
        if self._bIsFlat:
            return (0.0, 0.0, 1.0)

        # Optimized spherical case: normal points toward center of curvature
        if self._bIsSpherical:
            # Normal: -curv*(x, y) + (1-curv*z) in z
            nx = -x * self._dCurvature
            ny = -y * self._dCurvature
            nz = 1.0 - z * self._dCurvature
            return (nx, ny, nz)

        # General conic/aspheric case
        h2 = x * x + y * y

        if h2 > 1e-20:
            # Compute dTangRadius (tangent slope multiplied by radius h)
            if not self._bIsAspheric:
                # Conic case: use trick with z to avoid extra sqrt
                # dTangRadius = z*(2.0 + z²*(conic+1)*curv/(curv*h2-z))
                curv_h2 = self._dCurvature * h2
                denom = curv_h2 - z
                if abs(denom) > 1e-20:
                    dTangRadius = z * (2.0 + (z * z * (self._dConic + 1.0) * self._dCurvature) / denom)
                else:
                    dTangRadius = 0.0
            else:
                # Aspheric case: standard formula with sqrt
                dA = self._dCurvature
                dB = self._dCurvature * self._dCurvature * (self._dConic + 1.0)
                dR = math.sqrt(max(0.0, 1.0 - dB * h2))

                if dR > 0:
                    dTangRadius = h2 * (2.0 * dA / (dR + 1.0) + dA * dB * h2 / dR / ((dR + 1.0) ** 2))
                else:
                    dTangRadius = 0.0

                # Add polynomial derivatives: d/dh²[R4*h⁴ + R6*h⁶ + ...]
                h4 = h2 * h2
                h8 = h4 * h4
                dTangRadius += 4.0 * self._dR4 * h4 + 6.0 * self._dR6 * h4 * h2 + 8.0 * self._dR8 * h8 + 10.0 * self._dR10 * h8 * h2

            # Normal: (-x*dTangRadius, -y*dTangRadius, h2)
            nx = -x * dTangRadius
            ny = -y * dTangRadius
            nz = h2
        else:
            # At surface center
            nx = 0.0
            ny = 0.0
            nz = 1.0

        return (nx, ny, nz)

    def stop_photon(self, p):
        """
        Find ray-surface intersection and stop photon at surface.
        Uses closed-form quadratic solver for conic surfaces, 
        Newton-Raphson refinement for aspheric surfaces.
        Matches Surface.cpp implementation.
        """
        if not p.is_valid():
            return

        if math.isinf(self._dCurvature):
            p.valid = False
            return

        # Ray direction perpendicular to surface
        if p.dz == 0.0:
            p.valid = False
            return

        tmin = -p.z / p.dz
        if self._bIsFlat and tmin < 0.0:
            p.valid = False
            return

        # Stop the photon at z=0 first (for flat case or initial guess)
        p.x += tmin * p.dx
        p.y += tmin * p.dy
        p.z = 0.0

        if self._bIsFlat or self._bIsPerfect:
            p.valid = self.update_auto_diameter(p.x, p.y)
            return

        # ===== Conic case: solve quadratic equation =====
        # For conic surface: t²A + tB + C = 0
        # where p.z = 0 at this point
        dA = self._dCurvature * (p.dx*p.dx + p.dy*p.dy + (self._dConic + 1.0) * p.dz*p.dz)
        dB = 2.0 * (self._dCurvature * (p.x*p.dx + p.y*p.dy) - p.dz)
        dC = self._dCurvature * (p.x*p.x + p.y*p.y)

        tfinal = 0.0

        if abs(dA) < 1e-20:
            # Linear case: t*dB + dC = 0
            if abs(dB) < 1e-20:
                p.valid = False
                return
            tfinal = -dC / dB
        else:
            # Quadratic case: solve with stable formula
            dB2 = dB * dB
            delta = dB2 - 4.0 * dC * dA

            if delta < 0.0:
                p.valid = False
                return

            # Use stable formula to avoid cancellation
            sqrt_delta = math.sqrt(delta)
            if abs(delta - dB2) > 1e-20:  # normal case
                t1 = (2.0 * dC) / (sqrt_delta - dB) if abs(sqrt_delta - dB) > 1e-20 else 0.0
                t2 = (2.0 * dC) / (-sqrt_delta - dB) if abs(-sqrt_delta - dB) > 1e-20 else 0.0
            else:
                # delta ≈ dB²: use approximation
                if abs(dB) > 1e-20:
                    t1 = -dC / dB
                    t2 = 10.0 * t1  # to prefer t1
                else:
                    p.valid = False
                    return

            # Select t with smallest absolute value (closest to surface center)
            if t1*t1 < t2*t2:
                tfinal = t1
            else:
                tfinal = t2

        # Check if intersection is in future of photon
        if tmin + tfinal < 0.0:
            p.valid = False
            return

        p.x += p.dx * tfinal
        p.y += p.dy * tfinal
        p.z += p.dz * tfinal

        p.valid = self.update_auto_diameter(p.x, p.y)

        # ===== Aspheric refinement: Newton iterations =====
        if not self._bIsAspheric:
            return

        NB_ITER = 40
        RESOLUTION = 1e-7  # 0.1 nm resolution

        x = p.x
        y = p.y
        old_t = 0.0

        for iteration in range(NB_ITER):
            # Reproject onto aspheric surface
            z_proj, valid = self.compute_z(x, y)
            if not valid:
                p.valid = False
                return

            # Compute normal at this point
            nx, ny, nz = self.compute_normal(x, y, z_proj)

            # Plane equation: x*nx + y*ny + z*nz + d = 0
            d = -(x*nx + y*ny + z_proj*nz)

            # Newton step: intersect plane with ray
            denom = p.dx*nx + p.dy*ny + p.dz*nz
            if abs(denom) < 1e-20:
                break

            t = (-d - p.x*nx - p.y*ny - p.z*nz) / denom

            # Check convergence: distance moved
            dist_sq = (t - old_t)**2 * (p.dx*p.dx + p.dy*p.dy + p.dz*p.dz)
            if dist_sq < RESOLUTION**2:
                p.x = p.x + t * p.dx
                p.y = p.y + t * p.dy
                p.z = p.z + t * p.dz
                p.valid = self.update_auto_diameter(p.x, p.y)
                return

            x = p.x + t * p.dx
            y = p.y + t * p.dy
            old_t = t

        # Too many iterations
        p.valid = False

    def update_auto_diameter(self, dX, dY):
        r2 = dX * dX + dY * dY
        if self._bAutoDiameter:
            if r2 > self._dDiameter * self._dDiameter:
                self._dDiameter = math.sqrt(r2)
            return True
        return r2 <= self._dDiameter * self._dDiameter + 1e-6

    def compute_z(self, x, y):
        """
        Compute the surface sag (z coordinate) at position (x, y).
        Returns (z, valid) where valid=False if the point is off the surface.
        Matches Surface.cpp implementation.
        """
        # Flat or perfect surface
        if self._bIsFlat or self._bIsPerfect:
            return (0.0, True)

        h2 = x * x + y * y
        dA = 1.0 - (self._dConic + 1.0) * h2 * self._dCurvature * self._dCurvature

        if dA < 0.0:
            return (0.0, False)

        z = (h2 * self._dCurvature) / (1.0 + math.sqrt(dA))

        # Add aspheric polynomial terms
        if self._bIsAspheric:
            h4 = h2 * h2
            h6 = h4 * h2
            h8 = h4 * h4
            h10 = h4 * h6
            z += self._dR4 * h4 + self._dR6 * h6 + self._dR8 * h8 + self._dR10 * h10

        return (z, True)
