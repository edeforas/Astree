import numpy as np
import math

#################################################################################################
class Shape:
    def __init__(self):
        pass
    def stop(self,l):
        raise NotImplementedError()
    def add_normals(self,l):
        raise NotImplementedError()
#################################################################################################
class ShapeFlat(Shape):
    def __init__(self):
        super().__init__()

    def stop(self,l):
      #  l.valid= l.dz*l.z<0. # test if dz!=0 and tmin<0.
      #  l.remove_bad_photons()

        # stop the photon at z=0
        tmin=-l.z/l.dz
        l.x+=tmin*l.dx
        l.y+=tmin*l.dy
        l.z*=0.

    def add_normals(self,l):
        l.nx=np.zeros(l.x.shape)
        l.ny=np.zeros(l.x.shape)
        l.nz=np.ones(l.x.shape) # normals are not oriented
#################################################################################################
class ShapeConical(Shape):
    def __init__(self,rcurv,conic=0.):
        super().__init__()
        self.rcurv=rcurv
        self.curvature=1./rcurv
        self.conic=conic
    
    def stop(self,l):

        # stop the photon at z=0
        tflat=-l.z/l.dz
        l.x+=tflat*l.dx
        l.y+=tflat*l.dy
        l.z=0.

        #intersect with conic
        dA=self.curvature*(l.dx**2+l.dy**2)+(self.conic+1.)*(l.dz**2)
        dB=2.*(self.curvature*(l.x*l.dx+l.y*l.dy)-l.dz)
        dC=self.curvature*(l.x**2+l.y**2)
        delta=dB**2-4.*dC*dA
        l.valid= delta>=0.
        delta[l.valid==False]=0 # to avoid computing with Nan
        sqrtDelta=np.sqrt(delta)

        #select smaller solution
        div1=sqrtDelta-dB
        div2=-sqrtDelta-dB
        divfinal=np.where(np.abs(div1)<np.abs(div2),div1,div2)
        l.valid=np.logical_and(l.valid,(divfinal!=0))
        t=(2.*dC)/np.where(divfinal!=0.,divfinal,1.)

        l.valid=np.logical_and(l.valid,t+tflat>0.) # in the future of the photon

        l.x+=l.dx*t
        l.y+=l.dy*t
        l.z+=l.dz*t

""""
WAS:
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

"""

def add_normals(self,l):
    pass

#################################################################################################
def normalize(x, y, z):
    dNorm2=x**2+y**2+z**2

    if dNorm2!=0.:
        dNorm2=1./math.sqrt(dNorm2)
        x*=dNorm2
        y*=dNorm2
        z*=dNorm2
    
    return x,y,z
#################################################################################################
