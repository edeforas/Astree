# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <cassert>
#include <cmath>
#include <sstream>

#include "Surface.h"
#include "Photon.h"
#include "Light.h"
#include "GlassManager.h"
#include "Glass.h"
#include "Vector3D.h"

#define NB_ITER_STOP_NEWTON 40
#define RESOLUTION_STOP_NEWTON 1e-7 #0.1 nm=1e-7 mm  resolution

inline double sqr(double a)
    return a*a


#######################################
Surface.Surface():
    _sType("void")
    _pMaterial=0
    _pMaterialNext=0
    _pMaterialPrev=0
    _x=0.
    _y=0.
    _z=0.

    _dDiameter=0
    _dDiameter2=0

    _dInnerDiameter=0
    _dInnerDiameter2=0

    _bAutoDiameter=False
    _bAutoInnerDiameter=False

    _dConic=0.
    _dCurvature=0.

    _dR4=0.
    _dR6=0.
    _dR8=0.
    _dR10=0.

    _bIsPerfect=False
    update_geometry()

#######################################
Surface.Surface( Surface &rSurf):
    _sType("void")
    _pMaterial=0
    _pMaterialNext=0
    _pMaterialPrev=0
    _x=0.
    _y=0.
    _z=0.

    _dDiameter=0
    _dDiameter2=0

    _dInnerDiameter=0
    _dInnerDiameter2=0

    _bAutoDiameter=False
    _bAutoInnerDiameter=False

    _dConic=0.
    _dCurvature=0.

    _dR4=0.
    _dR6=0.
    _dR8=0.
    _dR10=0.

    operator=(rSurf)

#######################################
Surface& Surface.operator=( Surface& rSurf)
    _pMaterialNext=0
    _pMaterialPrev=0

    set_type(rSurf.type())
    set_conic(rSurf.conic())
    set_radius_curvature(rSurf.radius_curvature())

    set_R4(rSurf.R4())
    set_R6(rSurf.R6())
    set_R8(rSurf.R8())
    set_R10(rSurf.R10())

    set_comment(rSurf.comment())

    set_x(rSurf.x())
    set_y(rSurf.y())
    set_z(rSurf.z())

    set_diameter(rSurf.diameter())
    set_auto_diameter(rSurf.get_auto_diameter())

    set_inner_diameter(rSurf.inner_diameter())
    set_auto_inner_diameter(rSurf.get_auto_inner_diameter())

    return *self

#######################################
Surface.~Surface()
    delete _pMaterial

#######################################
def set_diameter(self, dDiameter):
    if _bAutoDiameter and (dDiameter<_dDiameter) :
        return

    _dDiameter=dDiameter
    _dDiameter2=sqr(_dDiameter)

#######################################
def set_auto_diameter(self, bAutoDiameter):
    _bAutoDiameter=bAutoDiameter

    if _bAutoDiameter:
        _dDiameter=0
        _dDiameter2=0


#######################################
def get_auto_diameter(self):
    return _bAutoDiameter

#######################################
def diameter(self):
{  
    return _dDiameter

#######################################
def set_inner_diameter(self, dInnerDiameter):
    if _bAutoInnerDiameter and (dInnerDiameter>_dInnerDiameter) :
        return

    _dInnerDiameter=dInnerDiameter
    _dInnerDiameter2=sqr(_dInnerDiameter)

#######################################
def set_auto_inner_diameter(self, bAutoInnerDiameter):
    _bAutoInnerDiameter=bAutoInnerDiameter

    if _bAutoInnerDiameter:
        _dInnerDiameter=MAX_DIAMETER
        _dInnerDiameter2=MAX_DIAMETER*MAX_DIAMETER


#######################################
def get_auto_inner_diameter(self):
    return _bAutoInnerDiameter

#######################################
def inner_diameter(self):
    return _dInnerDiameter

#######################################
def set_x(self, dx):
    _x=dx

#######################################
def x(self):
    return _x

#######################################
def set_y(self, dy):
    _y=dy

#######################################
def y(self):
    return _y

#######################################
def set_z(self, dz):
    _z=dz

#######################################
def z(self):
    return _z

#######################################
def update_auto_diameter(self, dX, dY):
    double dDiameterSq=4.*(sqr(dX)+sqr(dY))
    bool bInDiameter=(dDiameterSq<=_dDiameter2+LARGER_RADIUS_CHECK_TOLERANCIS); #TODO
    bool bInInnerDiameter=(dDiameterSq>=_dInnerDiameter2-LARGER_RADIUS_CHECK_TOLERANCIS); #TODO

    if bInDiameter and bInInnerDiameter:
        return True

    if (not _bAutoDiameter) and (not _bAutoInnerDiameter):
        return False

    double dDiameter=sqrt(dDiameterSq)

    if  (dDiameter>_dDiameter) and _bAutoDiameter:
        _dDiameter= dDiameter
        _dDiameter2= dDiameterSq
        bInDiameter=True


    if  (dDiameter<_dInnerDiameter) and _bAutoInnerDiameter:
        _dInnerDiameter= dDiameter
        _dInnerDiameter2= dDiameterSq
        bInInnerDiameter=True


    return (bInDiameter and bInInnerDiameter)

#######################################
def local_ref(self, p):
    # simple translation for now
    # TODO add rotation
    p.x-=x()
    p.y-=y()
    p.z-=z()

#######################################
def set_type(self, sType):
    assert(sType!="")
    if  (sType!="reflect") and (sType!="stop") and (sType!="image") and (sType!="image_infinite") and (sType!="transmit") and (sType!="void") and (sType!="perfect_lens") and (sType!="perfect_mirror") :
        #test if glass exist
        bool bExist=GlassManager.singleton().exist(sType)
        if not bExist:
            set_comment("Glass "+sType+" doesn't exist in catalognot ")

        GlassManager.singleton().destroy(_pMaterial)
        _pMaterial=GlassManager.singleton().create(sType)
		_sType = _pMaterial.name(); #overwrite type with glass name in case of glass replacement

	else:
		_pMaterial = 0
		_sType = sType


	_bIsPerfect = ((_sType == "perfect_lens") or (_sType == "perfect_mirror"))

    return True

#######################################
def type(self):
    return _sType

#######################################
def global_ref(self, p):
    # simple translation for now
    # TODO add rotation
    p.x+=x()
    p.y+=y()
    p.z+=z()

#######################################
def receive(self, l):
    assert(_sType!="")
    if  (_sType=="reflect") or (_sType=="perfect_mirror") :
        reflect(l)
    elif  (_sType=="stop") or ( _sType=="image" )or ( _sType=="image_infinite" ) :
        stop(l)
    elif _sType=="void":
        ; # void surface: do nothing
    else #all glasses or "perfect_lens"
        transmit(l)

#######################################
# stop photons
def stop(self, l):
    for (int i=0;i<l.nb_photon();i++)
        Photon& p=l.get_photon(i)

        if not p.is_valid():
            continue

        local_ref(p)
        stop_photon(p)
        global_ref(p)


#######################################
# reflect photons
def reflect(self, l):
    for (int i=0;i<l.nb_photon();i++)
        Photon& p=l.get_photon(i)

        if not p.is_valid():
            continue

        local_ref(p)
        reflect_photon(p)
        global_ref(p)


#######################################/
# transmit photons
def transmit(self, l):
    _pMaterialPrev=&(l.material())
    _pMaterialNext=_pMaterial

    assert(_pMaterialPrev)

    for (int i=0;i<l.nb_photon();i++)
        Photon& p=l.get_photon(i)

        if not p.is_valid():
            continue

        local_ref(p)
        transmit_photon(p)
        global_ref(p)


    if  (_pMaterial!=0)  and (_bIsPerfect==False):
        l.set_material(_pMaterial)

#######################################
def transmit_photon(self, p):
    if not p.is_valid():
        return

    stop_photon(p)
    if not p.is_valid():
        return

    if _bIsPerfect:
        #TODO optimise and merge tests
        if(p.dz==0.) #no intersection
            p.valid=False
            return


        #compute AP
        if  (_dCurvature<CURVATURE_FLAT) and (_dCurvature>-CURVATURE_FLAT):
            return; #flat lens

        double dFocal=0.5/_dCurvature
        double t=dFocal/p.dz

        if p.dz<0.:
            t=-t

        double ax=p.dx*t
        double ay=p.dy*t
        double az=p.dz*t

        p.dx=ax-p.x
        p.dy=ay-p.y
        p.dz=az-p.z

        if dFocal<0.:
            p.dx=-p.dx
            p.dy=-p.dy
            p.dz=-p.dz


        return


    assert(_pMaterialNext!=0)
    assert(_pMaterialPrev!=0)

    double nx,ny,nz

    #compute the normal in p (center distance : ro)
    double dRadiusSq=sqr(p.x)+sqr(p.y)
    if dRadiusSq!=0.:
        compute_normal(p.x,p.y,p.z,nx,ny,nz)
        Vector3D.normalize(nx,ny,nz)

    else # at surface center or flat surface
        nx=0.
        ny=0.
        nz=1.


    double u=nx*p.dx+ny*p.dy+nz*p.dz

    # compute with the normal (dx,dy,dz)
    # u is the projection of I on the normal

    #calcule de C1=u/I
    double dC12=u*u/(sqr(p.dx)+sqr(p.dy)+sqr(p.dz))
    double dRatioN=_pMaterialNext.index(p.lambda())/_pMaterialPrev.index(p.lambda()); #TODO store index and reuse
    double denom=sqr(dRatioN)+dC12-1.

    if denom<=0.:
    {  # total reflexion
        p.valid=False
        return


    double dT2T1=sqrt(dC12/denom)
    double k=(1.-dT2T1)*u

    p.dx=nx*k+dT2T1*p.dx
    p.dy=ny*k+dT2T1*p.dy
    p.dz=nz*k+dT2T1*p.dz

#######################################
def reflect_photon(self, &p):
    if not p.is_valid():
        return

    stop_photon(p)
    if not p.is_valid():
        return

    if _bIsPerfect:
        #TODO optimise and merge tests
        if(p.dz==0.) #no intersection
            p.valid=False
            return


        #compute AP
        if  (_dCurvature<CURVATURE_FLAT) and (_dCurvature>-CURVATURE_FLAT):
            p.dz=-p.dz
            return; #flat mirror


        double dFocal=0.5/_dCurvature
        double t=dFocal/p.dz
        double ax=p.dx*t
        double ay=p.dy*t
        double az=p.dz*t

        #TODO optimise tests with sign(t)
        if _dCurvature>0.:
            if p.dz>0.:
                p.dx=+ax+p.x
                p.dy=+ay+p.y
                p.dz=-az-p.z

            else:
                p.dx=-ax-p.x
                p.dy=-ay-p.y
                p.dz=+az-p.z


        else:
            if p.dz>0.:
                p.dx=-ax-p.x
                p.dy=-ay-p.y
                p.dz=+az-p.z

            else:
                p.dx=+ax+p.x
                p.dy=+ay+p.y
                p.dz=-az-p.z



        return


    double nx,ny,nz
    compute_normal(p.x,p.y,p.z,nx,ny,nz)

    # compute with the normal (nx,ny,nz)
    double dRayonSq=sqr(nx)+sqr(ny)+sqr(nz)
    assert(dRayonSq>0.)
    double u=2.*(nx*p.dx+ny*p.dy+nz*p.dz)/dRayonSq
    p.dx-=nx*u
    p.dy-=ny*u
    p.dz-=nz*u

#######################################/
def set_comment(self, sComment):
    _sComment=sComment

#######################################/
def comment(self):
    return _sComment

#######################################/
def set_conic(self, dConic):
    _dConic=dConic
    update_geometry()

#######################################/
def conic(self):
    return _dConic

#######################################/
def update_geometry(self):
    _bIsFlat=False
    _bIsSpherical=False
    _bIsConic=False

    _bIsAspheric=( (_dR4!=0.) or (_dR6!=0.) or (_dR8!=0.) or (_dR10!=0.) )

    if _bIsAspheric:
        return

    if abs(_dCurvature)<=CURVATURE_FLAT:
        _bIsFlat=True
        return


    if _dConic!=0.:
        _bIsConic=True
    else:
        _bIsSpherical=True

#######################################
def is_aspheric(self):
    return _bIsAspheric

#######################################/
def stop_photon(self, p):
    if p.is_valid()==False:
        return

    if std.isinf(_dCurvature):
        p.valid=False
        return


    # flat case (intersect with z=0)
    if p.dz==0.:
        # TODO
        p.valid=False
        return


    double tmin=-p.z/p.dz
    if _bIsFlat and (tmin<0.) :
        # in the past of the photon
        p.valid=False
        return


    # TODO optimise in case of spherical

    # stop the photon in z=0
    p.x+=tmin*p.dx
    p.y+=tmin*p.dy
    p.z=0.

    if _bIsFlat or _bIsPerfect:
        p.valid=update_auto_diameter(p.x,p.y)
        return


    #compute the coef of the 2nd degree eq in t:
    #the equation is t^2A+tB+C=0
    # use p.z=0.
    double dA=_dCurvature*(sqr(p.dx)+sqr(p.dy)+(_dConic+1.)*sqr(p.dz))
    double dB=2.*(_dCurvature*(p.x*p.dx+p.y*p.dy)-p.dz)
    double dC=_dCurvature*(sqr(p.x)+sqr(p.y))

    double tfinal

    if dA==0.:
        if dB==0.:
            p.valid=False
            return


        #the equation is now : t*dB+dC=0 so:
        tfinal=-dC/dB

    else #dA!=0
        #solve the equation
        double dB2=dB*dB
        double delta=dB2-4.*dC*dA

        if delta<0.:
            p.valid=False
            return


        double t1,t2

        if (delta!=dB2) # TODO enhance test
            double sqrtDelta=sqrt(delta)
            t1=(2.*dC)/(+sqrtDelta-dB)
            t2=(2.*dC)/(-sqrtDelta-dB)

        else:
            # delta~=dB2
            # use approximate solution:
            if dB!=0.:
                t1=-dC/dB
                t2=10.*t1; # to choose  t1

            else:
				p.valid=False;  # bug if we are here
                return



        # select t that gives the lowest abs(z)
        #t1ok=abs(z+t1.*dz)<abs(z+t2.*dz); #oct ver
        if (t1*t1<t2*t2) # todo optimize
            tfinal=t1
        else:
            tfinal=t2


    # check if intersection is in the futur of the photon
    if  tmin+tfinal<0 :
        p.valid=False
        return


    p.x+=p.dx*tfinal
    p.y+=p.dy*tfinal
    p.z+=p.dz*tfinal

    assert(p.is_valid())

    p.valid=update_auto_diameter(p.x,p.y)

    if not _bIsAspheric:
        return

    #aspheric mode
    #p.x,p.y,p.z is already a good approximation of the surface (as a conic), make some newton step

    double x=p.x
    double y=p.y
    # double z=p.z
    double dOldT=0
    for(int iLoop=0;iLoop<NB_ITER_STOP_NEWTON;iLoop++)
        #reproject z on aspheric curve
        double zproj
        compute_z(x,y,zproj)

        #compute normal on aspheric surface
        double nx,ny,nz
        compute_normal(x,y,zproj,nx,ny,nz)

        #compute the d value to have the plane x*nx+y*ny+z*nz+d=0
        double d=-(x*nx+y*ny+zproj*nz)

        #compute the intersect of self plane and the line defined by p (one newton step)
        double t=(-d-p.x*nx-p.y*ny-p.z*nz)/(p.dx*nx+p.dy*ny+p.dz*nz); #TODO tester !=0
        double distSQ=sqr(t-dOldT)*(sqr(p.dx)+sqr(p.dy)+sqr(p.dz))
        x=p.x+t*p.dx
        y=p.y+t*p.dy
        double z=p.z+t*p.dz

        if distSQ<sqr(RESOLUTION_STOP_NEWTON):
            p.x=x
            p.y=y
            p.z=z

            p.valid=update_auto_diameter(p.x,p.y)
            return


        dOldT=t


    #too many iterations
    p.valid=False
    return

#######################################
def compute_normal(self, x, y, z, nx, ny, &nz):
    if _bIsFlat:
        nx=0.
        ny=0.
        nz=1.
        return True


    if _bIsSpherical:
        assert(x*x+y*y<=1./sqr(_dCurvature)); #check the point is on the surface

        # optimisation in case of spherical shape
        # the normal target the center sphere at (0,0,1/curv) from (x,y,z)
        # so it is the vector (0-x,0-y 1/curv-z)
        # we multiply by curv to avoid the division

        nx=-x*_dCurvature
        ny=-y*_dCurvature
        nz=1.-z*_dCurvature
        return True


    #the distance from revolution center is h=sqrt(x*x+y*y)
    # so square(distance) is h2=x*x+y*y
    double h2=sqr(x)+sqr(y)

    # here, compute the tangent vector, by radius h; self is ok since we are only interested by its direction
    # and not its norm. This way the computation take one sqrt less:
    # at first, tangent vector is [ x/h, y/h, dz/dh]
    # if me multiply by h : its became [ x, y, h*dz/dh ] wich is easier to compute due to the dh in the
    # formula of dz/dh, give h*dz/dh . h2*dw/dh and dw is simpler

    if h2>0.:
        double dTangRadius
        if not _bIsAspheric:
            #conic case
            #dTangRadius est la pente par rapport au rayon (multiplied by radius h)
            # there is a trick in the conic case, reuse z to avoid computing the sqrt again
            dTangRadius=z*(2.+sqr(z)*(_dConic+1.)*_dCurvature/(_dCurvature*h2-z))

        else:
            #aspheric case

            #alternative method with one sqrt
            double dA=_dCurvature
            double dB=sqr(_dCurvature)*(_dConic+1.)
            double dR=sqrt(1.-dB*h2)

            # real conic derivative versus radius (multiplied by radius h)
            dTangRadius=h2*(2.*dA/(dR+1.)+dA*dB*h2/dR/sqr((dR+1.))); #TODO optimise

            # add the polynomial derivate (multiplied by radius h)
            double h4=sqr(h2)
            double h8=sqr(h4)
            dTangRadius+=4.*_dR4*h4+6.*_dR6*h4*h2+8.*_dR8*h8+10.*_dR10*h8*h2;  #TODO use horner rule


        # tangent vector is parallel to circumference
        # dot product to compute the normal
        nx=-x*dTangRadius
        ny=-y*dTangRadius
        nz=h2

    else:
        nx=0.
        ny=0.
        nz=1.

    return True

#######################################
def compute_z(self, x, y, &z):
    if _bIsFlat or _bIsPerfect:
        z=0.
        return True


    double h2=sqr(x)+sqr(y)
    double dA=1.-(_dConic+1.)*h2*sqr(_dCurvature)
    if dA<0.:
        return False

    z= (h2*_dCurvature)/(1.+sqrt(dA))
    assert(not isnan(z))

    if _bIsAspheric:
        double h4=sqr(h2)
        double h6=h4*h2
        z+=_dR4*h4+_dR6*h6+_dR8*sqr(h4)+_dR10*h4*h6; #TODO use horner form


    return True

#######################################
def set_radius_curvature(self, dRadiusCurvature):
    _dCurvature=1./dRadiusCurvature
    update_geometry()

#######################################
def set_curvature(self, dCurvature):
    _dCurvature=dCurvature
    update_geometry()

#######################################
def radius_curvature(self):
    return 1./_dCurvature

#######################################
def curvature(self):
    return _dCurvature

#######################################
def set_R4(self, dR4):
    _dR4=dR4
    update_geometry()

#######################################
def set_R6(self, dR6):
    _dR6=dR6
    update_geometry()

#######################################
def set_R8(self, dR8):
    _dR8=dR8
    update_geometry()

#######################################
def set_R10(self, dR10):
    _dR10=dR10
    update_geometry()

#######################################
def R4(self):
    return _dR4

#######################################
def R6(self):
    return _dR6

#######################################
def R8(self):
    return _dR8

#######################################
def R10(self):
    return _dR10

#######################################
