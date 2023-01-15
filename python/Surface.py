import Vector3D
import Light
import GlassManager
import math

MAX_DIAMETER= 1.e10
LARGER_RADIUS_CHECK_TOLERANCIS= 1e-6 #1 nm
CURVATURE_FLAT =1e-20

NB_ITER_STOP_NEWTON =40
RESOLUTION_STOP_NEWTON =1e-7 #//0.1 nm=1e-7 mm  resolution


class Surface:
    def __init__(self):
        self._sType = "void"
        self._pMaterial = None
        self._pMaterialNext = None
        self._pMaterialPrev = None
        self._x = 0.0
        self._y = 0.0
        self._z = 0.0

        self._dDiameter = 0
        self._dDiameter2 = 0

        self._dInnerDiameter = 0
        self._dInnerDiameter2 = 0

        self._bAutoDiameter = False
        self._bAutoInnerDiameter = False

        self._dConic = 0.0
        self._dCurvature = 0.0

        self._dR4 = 0.0
        self._dR6 = 0.0
        self._dR8 = 0.0
        self._dR10 = 0.0

        self._bIsPerfect = False
        self.update_geometry()

    def __copy__(self):
        new_surface = Surface()
        new_surface._pMaterial = None
        new_surface._pMaterialNext = None
        new_surface._pMaterialPrev = None
        new_surface._x = 0.0
        new_surface._y = 0.0
        new_surface._z = 0.0

        new_surface._dDiameter = 0
        new_surface._dDiameter2 = 0

        new_surface._dInnerDiameter = 0
        new_surface._dInnerDiameter2 = 0

        new_surface._bAutoDiameter = False
        new_surface._bAutoInnerDiameter = False

        new_surface._dConic = 0.0
        new_surface._dCurvature = 0.0

        new_surface._dR4 = 0.0
        new_surface._dR6 = 0.0
        new_surface._dR8 = 0.0
        new_surface._dR10 = 0.0

        return new_surface

    def __assign__(self, rSurf):
        self._pMaterialNext = None
        self._pMaterialPrev = None

        self.set_type(rSurf.type())
        self.set_conic(rSurf.conic())
        self.set_radius_curvature(rSurf.radius_curvature())

        self.set_R4(rSurf.R4())
        self.set_R6(rSurf.R6())
        self.set_R8(rSurf.R8())
        self.set_R10(rSurf.R10())

        self.set_comment(rSurf.comment())

        self.set_x(rSurf.x())
        self.set_y(rSurf.y())
        self.set_z(rSurf.z())

        self.set_diameter(rSurf.diameter())
        self.set_auto_diameter(rSurf.get_auto_diameter())

        self.set_inner_diameter(rSurf.inner_diameter())
        self.set_auto_inner_diameter(rSurf.get_auto_inner_diameter())

        return self

    def __del__(self):
        del self._pMaterial

    def set_diameter(self, dDiameter):
        if self._bAutoDiameter and (dDiameter < self._dDiameter):
            return

        self._dDiameter = dDiameter
        self._dDiameter2 = self.sqr(self._dDiameter)

    def set_auto_diameter(self, bAutoDiameter):
        self._bAutoDiameter = bAutoDiameter

        if self._bAutoDiameter:
            self._dDiameter = 0
            self._dDiameter2 = 0

    def get_auto_diameter(self):
        return self._bAutoDiameter

    def diameter(self):
        return self._dDiameter

    def set_inner_diameter(self, dInnerDiameter):
        if self._bAutoInnerDiameter and (dInnerDiameter > self._dInnerDiameter):
            return

        self._dInnerDiameter = dInnerDiameter
        self._dInnerDiameter2 = self.sqr(self._dInnerDiameter)

    def set_auto_inner_diameter(self, bAutoInnerDiameter):
        self._bAutoInnerDiameter = bAutoInnerDiameter

        if self._bAutoInnerDiameter:
            self._dInnerDiameter = MAX_DIAMETER
            self._dInnerDiameter2 = MAX_DIAMETER * MAX_DIAMETER

    def get_auto_inner_diameter(self):
        return self._bAutoInnerDiameter

    def inner_diameter(self):
        return self._dInnerDiameter

    def set_x(self, dx):
        self._x = dx

    def x(self):
        return self._x

    def update_geometry(self):
        # Placeholder for the actual implementation
        pass

    def sqr(self, value):
        return value * value

    def set_y(self, dy):
        self._y = dy

    def y(self):
        return self._y

    def set_z(self, dz):
        self._z = dz

    def z(self):
        return self._z

    def update_auto_diameter(self, dX, dY):
        dDiameterSq = 4.0 * (dX ** 2 + dY ** 2)
        bInDiameter = (dDiameterSq <= self._dDiameter2 + LARGER_RADIUS_CHECK_TOLERANCIS)
        bInInnerDiameter = (dDiameterSq >= self._dInnerDiameter2 - LARGER_RADIUS_CHECK_TOLERANCIS)

        if bInDiameter and bInInnerDiameter:
            return True

        if not self._bAutoDiameter and not self._bAutoInnerDiameter:
            return False

        dDiameter = math.sqrt(dDiameterSq)

        if dDiameter > self._dDiameter and self._bAutoDiameter:
            self._dDiameter = dDiameter
            self._dDiameter2 = dDiameterSq
            bInDiameter = True

        if dDiameter < self._dInnerDiameter and self._bAutoInnerDiameter:
            self._dInnerDiameter = dDiameter
            self._dInnerDiameter2 = dDiameterSq
            bInInnerDiameter = True

        return bInDiameter and bInInnerDiameter

    def local_ref(self, p):
        p.x -= self.x()
        p.y -= self.y()
        p.z -= self.z()

    def set_type(self, sType):
        assert sType != ""
        if sType not in ["reflect", "stop", "image", "image_infinite", "transmit", "void", "perfect_lens", "perfect_mirror"]:
            bExist = GlassManager.singleton().exist(sType)
            if not bExist:
                self.set_comment(f"Glass {sType} doesn't exist in catalog!")

            GlassManager.singleton().destroy(self._pMaterial)
            self._pMaterial = GlassManager.singleton().create(sType)
            self._sType = self._pMaterial.name()  # overwrite type with glass name in case of glass replacement
        else:
            self._pMaterial = None
            self._sType = sType

        self._bIsPerfect = (self._sType == "perfect_lens" or self._sType == "perfect_mirror")

        return True

    def type(self):
        return self._sType

    def global_ref(self, p):
        p.x += self.x()
        p.y += self.y()
        p.z += self.z()

    def receive(self, l):
        assert self._sType != ""
        if self._sType in ["reflect", "perfect_mirror"]:
            self.reflect(l)
        elif self._sType in ["stop", "image", "image_infinite"]:
            self.stop(l)
        elif self._sType == "void":
            pass  # void surface: do nothing
        else:  # all glasses or "perfect_lens"
            self.transmit(l)

    def stop(self, l):
        for i in range(l.nb_photon()):
            p = l.get_photon(i)

            if not p.is_valid():
                continue

            self.local_ref(p)
            self.stop_photon(p)
            self.global_ref(p)
            
    def reflect(self, l):
        for i in range(l.nb_photon()):
            p = l.get_photon(i)

            if not p.is_valid():
                continue

            self.local_ref(p)
            self.reflect_photon(p)
            self.global_ref(p)

    def transmit(self, l):
        self._pMaterialPrev = l.material()
        self._pMaterialNext = self._pMaterial

        assert self._pMaterialPrev

        for i in range(l.nb_photon()):
            p = l.get_photon(i)

            if not p.is_valid():
                continue

            self.local_ref(p)
            self.transmit_photon(p)
            self.global_ref(p)

        if self._pMaterial != 0 and not self._bIsPerfect:
            l.set_material(self._pMaterial)

    def transmit_photon(self, p):
        if not p.is_valid():
            return

        self.stop_photon(p)
        if not p.is_valid():
            return

        if self._bIsPerfect:
            if p.dz == 0.0:  # no intersection
                p.valid = False
                return

            if self._dCurvature < CURVATURE_FLAT and self._dCurvature > -CURVATURE_FLAT:
                return  # flat lens

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

        assert self._pMaterialNext != 0
        assert self._pMaterialPrev != 0

        # compute the normal in p (center distance : ro)
        dRadiusSq = p.x ** 2 + p.y ** 2
        if dRadiusSq != 0.0:
            nx, ny, nz = self.compute_normal(p.x, p.y, p.z)
            Vector3D.normalize(nx, ny, nz)
        else:  # at surface center or flat surface
            nx = 0.0
            ny = 0.0
            nz = 1.0

        u = nx * p.dx + ny * p.dy + nz * p.dz

        # calcule de C1=u/I
        dC12 = u * u / (p.dx ** 2 + p.dy ** 2 + p.dz ** 2)
        dRatioN = self._pMaterialNext.index(p.get_wavelength()) / self._pMaterialPrev.index(p.get_wavelength())  # TODO store index and reuse
        denom = dRatioN ** 2 + dC12 - 1.0

        if denom <= 0.0:  # total reflexion
            p.valid = False
            return

        dT2T1 = (dC12 / denom) ** 0.5
        k = (1.0 - dT2T1) * u

        p.dx = nx * k + dT2T1 * p.dx
        p.dy = ny * k + dT2T1 * p.dy
        p.dz = nz * k + dT2T1 * p.dz

    def copy(self):
        new_surface = Surface()
        new_surface._z = self._z
        new_surface._comment = self._comment
        new_surface._auto_diameter = self._auto_diameter
        new_surface._auto_inner_diameter = self._auto_inner_diameter
        new_surface._inner_diameter = self._inner_diameter
        new_surface._radius_curvature = self._radius_curvature
        new_surface._aspheric = self._aspheric
        return new_surface

    def set_z(self, z: float):
        self._z = z

    def z(self) -> float:
        return self._z

    def set_comment(self, comment: str):
        self._comment = comment

    def comment(self) -> str:
        return self._comment

    def set_auto_diameter(self, auto: bool):
        self._auto_diameter = auto

    def get_auto_diameter(self) -> bool:
        return self._auto_diameter

    def set_auto_inner_diameter(self, auto: bool):
        self._auto_inner_diameter = auto

    def get_auto_inner_diameter(self) -> bool:
        return self._auto_inner_diameter

    def inner_diameter(self) -> float:
        return self._inner_diameter

    def is_aspheric(self) -> bool:
        return self._aspheric

    def set_radius_curvature(self, radius: float):
        self._radius_curvature = radius

    def receive(self, light):
        # Placeholder implementation
        pass
##############################################################
    def reflect_photon(self, photon):
        if not photon.is_valid():
            return

        self.stop_photon(photon)
        if not photon.is_valid():
            return

        if self._bIsPerfect:
            # TODO optimise and merge tests
            if photon.dz == 0.0:  # no intersection
                photon.valid = False
                return

            # compute AP
            if (self._dCurvature < CURVATURE_FLAT) and (self._dCurvature > -CURVATURE_FLAT):
                photon.dz = -photon.dz
                return  # flat mirror

            focal_distance = 0.5 / self._dCurvature
            t = focal_distance / photon.dz
            ax = photon.dx * t
            ay = photon.dy * t
            az = photon.dz * t

            # TODO optimise tests with sign(t)
            if self._dCurvature > 0.0:
                if photon.dz > 0.0:
                    photon.dx = +ax + photon.x
                    photon.dy = +ay + photon.y
                    photon.dz = -az - photon.z
                else:
                    photon.dx = -ax - photon.x
                    photon.dy = -ay - photon.y
                    photon.dz = +az - photon.z
            else:
                if photon.dz > 0.0:
                    photon.dx = -ax - photon.x
                    photon.dy = -ay - photon.y
                    photon.dz = +az - photon.z
                else:
                    photon.dx = +ax + photon.x
                    photon.dy = +ay + photon.y
                    photon.dz = -az - photon.z

            return

        normal_x, normal_y, normal_z = self.compute_normal(photon.x, photon.y, photon.z)

        # compute with the normal (normal_x, normal_y, normal_z)
        ray_squared = self.sqr(normal_x) + self.sqr(normal_y) + self.sqr(normal_z)
        assert ray_squared > 0.0
        u = 2.0 * (normal_x * photon.dx + normal_y * photon.dy + normal_z * photon.dz) / ray_squared
        photon.dx -= normal_x * u
        photon.dy -= normal_y * u
        photon.dz -= normal_z * u

    def set_comment(self, comment):
        self._sComment = comment

    def comment(self):
        return self._sComment

    def set_conic(self, conic_value):
        self._dConic = conic_value
        self.update_geometry()

    def conic(self):
        return self._dConic

    def update_geometry(self):
        self._bIsFlat = False
        self._bIsSpherical = False
        self._bIsConic = False

        self._bIsAspheric = (self._dR4 != 0.0 or self._dR6 != 0.0 or self._dR8 != 0.0 or self._dR10 != 0.0)

        if self._bIsAspheric:
            return

        if abs(self._dCurvature) <= CURVATURE_FLAT:
            self._bIsFlat = True
            return

        if self._dConic != 0.0:
            self._bIsConic = True
        else:
            self._bIsSpherical = True

    def is_aspheric(self):
        return self._bIsAspheric
##############################################################################

    def stop_photon(self,photon):
        if not photon.is_valid():
            return

        if math.isinf(self._dCurvature):
            photon.valid = False
            return

        # flat case (intersect with z=0)
        if photon.dz == 0.0:
            # TODO
            photon.valid = False
            return

        t_min = -photon.z / photon.dz
        if self._bIsFlat and t_min < 0.0:
            # in the past of the photon
            photon.valid = False
            return

        # TODO optimise in case of spherical

        # stop the photon in z=0
        photon.x += t_min * photon.dx
        photon.y += t_min * photon.dy
        photon.z = 0.0

        if self._bIsFlat or self._bIsPerfect:
            photon.valid = self.update_auto_diameter(photon.x, photon.y)
            return

        # compute the coef of the 2nd degree eq in t:
        # the equation is t^2A+tB+C=0
        # use photon.z=0.
        dA = self._dCurvature * (self.sqr(photon.dx) + self.sqr(photon.dy) + (self._dConic + 1.0) * self.sqr(photon.dz))
        dB = 2.0 * (self._dCurvature * (photon.x * photon.dx + photon.y * photon.dy) - photon.dz)
        dC = self._dCurvature * (self.sqr(photon.x) + self.sqr(photon.y))

        t_final = 0.0

        if dA == 0.0:
            if dB == 0.0:
                photon.valid = False
                return

            # the equation is now : t*dB+dC=0 so:
            t_final = -dC / dB
        else:  # dA != 0
            # solve the equation
            dB2 = dB * dB
            delta = dB2 - 4.0 * dC * dA

            if delta < 0.0:
                photon.valid = False
                return

            t1, t2 = 0.0, 0.0

            if delta != dB2:  # TODO enhance test
                sqrt_delta = math.sqrt(delta)
                t1 = (2.0 * dC) / (+sqrt_delta - dB)
                t2 = (2.0 * dC) / (-sqrt_delta - dB)
            else:
                # delta ~= dB2
                # use approximate solution:
                if dB != 0.0:
                    t1 = -dC / dB
                    t2 = 10.0 * t1  # to choose t1
                else:
                    photon.valid = False  # bug if we are here
                    return

            # select t that gives the lowest abs(z)
            if t1 * t1 < t2 * t2:  # todo optimize
                t_final = t1
            else:
                t_final = t2

        # check if intersection is in the future of the photon
        if t_min + t_final < 0:
            photon.valid = False
            return

        photon.x += photon.dx * t_final
        photon.y += photon.dy * t_final
        photon.z += photon.dz * t_final

        assert photon.is_valid()

        photon.valid = self.update_auto_diameter(photon.x, photon.y)

        if not self._bIsAspheric:
            return

        # aspheric mode
        # photon.x, photon.y, photon.z is already a good approximation of the surface (as a conic), but make some newton step

        x = photon.x
        y = photon.y
        # z = photon.z
        d_old_t = 0
        for i_loop in range(NB_ITER_STOP_NEWTON):
            # reproject z on aspheric curve
            z_proj = 0.0
            self.compute_z(x, y, z_proj)

            # compute normal on aspheric surface
            nx, ny, nz = 0.0, 0.0, 0.0
            self.compute_normal(x, y, z_proj, nx, ny, nz)

            # compute the d value to have the plane x*nx+y*ny+z*nz+d=0
            d = -(x * nx + y * ny + z_proj * nz)

            # compute the intersect of this plane and the line defined by photon (one newton step)
            t = (-d - photon.x * nx - photon.y * ny - photon.z * nz) / (photon.dx * nx + photon.dy * ny + photon.dz * nz)  # TODO tester !=0
            dist_sq = self.sqr(t - d_old_t) * (self.sqr(photon.dx) + self.sqr(photon.dy) + self.sqr(photon.dz))
            x = photon.x + t * photon.dx
            y = photon.y + t * photon.dy
            z = photon.z + t * photon.dz

            if dist_sq < self.sqr(RESOLUTION_STOP_NEWTON):
                photon.x = x
                photon.y = y
                photon.z = z

                photon.valid = self.update_auto_diameter(photon.x, photon.y)
                return

            d_old_t = t

        # too many iterations
        photon.valid = False
        return
    

    def compute_normal(self, x, y, z):
        normal_x = 0.0
        normal_y = 0.0
        normal_z = 0.0

        if self._bIsFlat:
            normal_z = 1.0
            return True, normal_x, normal_y, normal_z

        if self._bIsSpherical:
            assert x * x + y * y <= 1.0 / self.sqr(self._dCurvature)  # check the point is on the surface

            # optimization in case of spherical shape
            # the normal targets the center sphere at (0,0,1/curv) from (x,y,z)
            # so it is the vector (0-x,0-y, 1/curv-z)
            # we multiply by curv to avoid the division

            normal_x = -x * self._dCurvature
            normal_y = -y * self._dCurvature
            normal_z = 1.0 - z * self._dCurvature
            return True, normal_x, normal_y, normal_z

        # the distance from revolution center is h=sqrt(x*x+y*y)
        # so square(distance) is h2=x*x+y*y
        h2 = self.sqr(x) + self.sqr(y)

        # here, we compute the tangent vector, multiplied by radius h; this is ok since we are only interested by its direction
        # and not its norm. This way the computation takes one sqrt less:
        # at first, the tangent vector is [ x/h, y/h, dz/dh]
        # if we multiply by h: it becomes [ x, y, h*dz/dh ] which is easier to compute due to the dh in the
        # formula of dz/dh, this gives h*dz/dh -> h2*dw/dh and dw is simpler

        if h2 > 0.0:
            if not self._bIsAspheric:
                # conic case
                # dTangRadius is the slope with respect to the radius (multiplied by radius h)
                # there is a trick in the conic case, we reuse z to avoid computing the sqrt again
                dTangRadius = z * (2.0 + self.sqr(z) * (self._dConic + 1.0) * self._dCurvature / (self._dCurvature * h2 - z))
            else:
                # aspheric case

                # alternative method with one sqrt
                dA = self._dCurvature
                dB = self.sqr(self._dCurvature) * (self._dConic + 1.0)
                dR = (1.0 - dB * h2) ** 0.5

                # real conic derivative versus radius (multiplied by radius h)
                dTangRadius = h2 * (2.0 * dA / (dR + 1.0) + dA * dB * h2 / dR / self.sqr((dR + 1.0)))  # TODO optimize

                # add the polynomial derivative (multiplied by radius h)
                h4 = self.sqr(h2)
                h8 = self.sqr(h4)
                dTangRadius += (4.0 * self._dR4 * h4 + 6.0 * self._dR6 * h4 * h2 +
                                 8.0 * self._dR8 * h8 + 10.0 * self._dR10 * h8 * h2)  # TODO use Horner's rule

            # tangent vector is parallel to circumference
            # dot product to compute the normal
            normal_x = -x * dTangRadius
            normal_y = -y * dTangRadius
            normal_z = h2
        else:
            normal_z = 1.0

        return True, normal_x, normal_y, normal_z

    def compute_z(self, x, y):
        if self._bIsFlat or self._bIsPerfect:
            return True, 0.0

        h2 = self.sqr(x) + self.sqr(y)
        dA = 1.0 - (self._dConic + 1.0) * h2 * self.sqr(self._dCurvature)
        if dA < 0.0:
            return False, None

        z = (h2 * self._dCurvature) / (1.0 + dA ** 0.5)
        assert not (z != z)  # check for NaN

        if self._bIsAspheric:
            h4 = self.sqr(h2)
            h6 = h4 * h2
            z += (self._dR4 * h4 + self._dR6 * h6 + self._dR8 * self.sqr(h4) + self._dR10 * h4 * h6)  # TODO use Horner form

        return True, z

    def set_radius_curvature(self, dRadiusCurvature):
        self._dCurvature = 1.0 / dRadiusCurvature
        self.update_geometry()

    def set_curvature(self, dCurvature):
        self._dCurvature = dCurvature
        self.update_geometry()

    def radius_curvature(self):
        return 1.0 / self._dCurvature

    def curvature(self):
        return self._dCurvature

    def sqr(self, value):
        return value * value

    def update_geometry(self):
        # Geometry updating logic goes here
        pass

    def set_R4(self, radius_4):
        self._dR4 = radius_4
        self.update_geometry()

    def set_R6(self, radius_6):
        self._dR6 = radius_6
        self.update_geometry()

    def set_R8(self, radius_8):
        self._dR8 = radius_8
        self.update_geometry()

    def set_R10(self, radius_10):
        self._dR10 = radius_10
        self.update_geometry()

    def R4(self):
        return self._dR4

    def R6(self):
        return self._dR6

    def R8(self):
        return self._dR8

    def R10(self):
        return self._dR10

    def update_geometry(self):
        # Placeholder for the geometry update logic
        pass        

class SurfaceParameterClone:
    def __init__(self):
        self.iSurface = 0
        self.iRefSurface = 0
        self.param = ""
        self.dGain = 0.0