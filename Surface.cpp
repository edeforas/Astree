// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <cassert>
#include <cmath>
#include <sstream>

#include "Surface.h"
#include "Vector3D.h"
#include "Photon.h"
#include "Light.h"
#include "MaterialManager.h"
#include "Material.h"

inline double sqr(double a) { return a*a; }

#define NB_ITER_STOP_NEWTON 40
#define RESOLUTION_STOP_NEWTON 1e-7 //0.1 nm=1e-7 mm  resolution

//////////////////////////////////////////////////////////////////////////////
Surface::Surface():
    _sType("void")
{
    _pMaterial=0;
    _pMaterialNext=0;
    _pMaterialPrev=0;
    _x=0.;
    _y=0.;
    _z=0.;

    _dDiameter=0;
    _dDiameter2=0;

    _dInnerDiameter=0;
    _dInnerDiameter2=0;

    _bAutoDiameter=false;
    _bAutoInnerDiameter=false;
    _bAutoFocus=false;

    _dConic=0.;
    _dCurvature=0.;

    _dR4=0.;
    _dR6=0.;
    _dR8=0.;
    _dR10=0.;

    _bIsPerfect=false;
    update_geometry();
}
//////////////////////////////////////////////////////////////////////////////
Surface::Surface(const Surface &rSurf):
    _sType("void")
{
    _pMaterial=0;
    _pMaterialNext=0;
    _pMaterialPrev=0;
    _x=0.;
    _y=0.;
    _z=0.;

    _dDiameter=0;
    _dDiameter2=0;

    _dInnerDiameter=0;
    _dInnerDiameter2=0;

    _bAutoDiameter=false;
    _bAutoInnerDiameter=false;
    _bAutoFocus=false;

    _dConic=0.;
    _dCurvature=0.;

    _dR4=0.;
    _dR6=0.;
    _dR8=0.;
    _dR10=0.;

    operator=(rSurf);
}
//////////////////////////////////////////////////////////////////////////////
Surface& Surface::operator=(const Surface& rSurf)
{
    _pMaterialNext=0;
    _pMaterialPrev=0;

    set_type(rSurf.type());
    set_conic(rSurf.conic());
    set_radius_curvature(rSurf.radius_curvature());

    set_R4(rSurf.R4());
    set_R6(rSurf.R6());
    set_R8(rSurf.R8());
    set_R10(rSurf.R10());

    set_comment(rSurf.comment());

    set_x(rSurf.x());
    set_y(rSurf.y());
    set_z(rSurf.z());

    set_diameter(rSurf.diameter());
    set_auto_diameter(rSurf.get_auto_diameter());

    set_inner_diameter(rSurf.inner_diameter());
    set_auto_inner_diameter(rSurf.get_auto_inner_diameter());

    set_autofocus(rSurf.get_autofocus());

    return *this;
}
//////////////////////////////////////////////////////////////////////////////
Surface::~Surface()
{
    delete _pMaterial;
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_diameter(double dDiameter)
{
    if(_bAutoDiameter && (dDiameter<_dDiameter) )
        return;

    _dDiameter=dDiameter;
    _dDiameter2=sqr(_dDiameter);
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_auto_diameter(bool bAutoDiameter)
{
    _bAutoDiameter=bAutoDiameter;

    if(_bAutoDiameter)
    {
        _dDiameter=0;
        _dDiameter2=0;
    }
}
//////////////////////////////////////////////////////////////////////////////
bool Surface::get_auto_diameter() const
{
    return _bAutoDiameter;
}
//////////////////////////////////////////////////////////////////////////////
double Surface::diameter() const
{  
    return _dDiameter;
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_inner_diameter(double dInnerDiameter)
{
    if(_bAutoInnerDiameter && (dInnerDiameter>_dInnerDiameter) )
        return;

    _dInnerDiameter=dInnerDiameter;
    _dInnerDiameter2=sqr(_dInnerDiameter);
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_auto_inner_diameter(bool bAutoInnerDiameter)
{
    _bAutoInnerDiameter=bAutoInnerDiameter;

    if(_bAutoInnerDiameter)
    {
        _dInnerDiameter=MAX_DIAMETER;
        _dInnerDiameter2=MAX_DIAMETER*MAX_DIAMETER;
    }
}
//////////////////////////////////////////////////////////////////////////////
bool Surface::get_auto_inner_diameter() const
{
    return _bAutoInnerDiameter;
}
//////////////////////////////////////////////////////////////////////////////
double Surface::inner_diameter() const
{
    return _dInnerDiameter;
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_x(double dx)
{
    _x=dx;
}
//////////////////////////////////////////////////////////////////////////////
double Surface::x() const
{
    return _x;
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_y(double dy)
{
    _y=dy;
}
//////////////////////////////////////////////////////////////////////////////
double Surface::y() const
{
    return _y;
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_z(double dz)
{
    _z=dz;
}
//////////////////////////////////////////////////////////////////////////////
double Surface::z() const
{
    return _z;
}
//////////////////////////////////////////////////////////////////////////////
bool Surface::verify_in_surface(double dX,double dY)
{
    double dDiameterSq=4.*(sqr(dX)+sqr(dY));
    bool bInDiameter=(dDiameterSq<=_dDiameter2+LARGER_RADIUS_CHECK_TOLERANCIS); //TODO
    bool bInInnerDiameter=(dDiameterSq>=_dInnerDiameter2-LARGER_RADIUS_CHECK_TOLERANCIS); //TODO

    if(bInDiameter && bInInnerDiameter)
        return true;

    if((!_bAutoDiameter) && (!_bAutoInnerDiameter))
        return false;

    double dDiameter=sqrt(dDiameterSq);

    if( (dDiameter>_dDiameter) && _bAutoDiameter)
    {
        _dDiameter= dDiameter;
        _dDiameter2= dDiameterSq;
        bInDiameter=true;
    }

    if( (dDiameter<_dInnerDiameter) && _bAutoInnerDiameter)
    {
        _dInnerDiameter= dDiameter;
        _dInnerDiameter2= dDiameterSq;
        bInInnerDiameter=true;
    }

    return (bInDiameter && bInInnerDiameter);
}
//////////////////////////////////////////////////////////////////////////////
void Surface::local_ref(Photon& p) const
{
    // translation simple de ref, en attendant le besoins de gerer + de choses
    // todo add rotation
    p.x-=x();
    p.y-=y();
    p.z-=z();
}
//////////////////////////////////////////////////////////////////////////////
bool Surface::set_type(string sType)
{
    assert(sType!="");
    if ( (sType!="reflect") && (sType!="stop") && (sType!="image") && (sType!="transmit") && (sType!="void") && (sType!="perfect_lens") && (sType!="perfect_mirror") )
    {
        MaterialManager::singleton().destroy(_pMaterial);
        _pMaterial=MaterialManager::singleton().create(sType);
        assert(_pMaterial!=0);
    }
    else
        _pMaterial=0;

    _bIsPerfect = ((sType=="perfect_lens") || (sType=="perfect_mirror"));

    _sType=sType;
    return true;
}
//////////////////////////////////////////////////////////////////////////////
string Surface::type() const
{
    return _sType;
}
//////////////////////////////////////////////////////////////////////////////
void Surface::global_ref(Photon& p) const
{
    // translation simple de ref, en attendant le besoins de gerer + de choses
    // todo add rotation
    p.x+=x();
    p.y+=y();
    p.z+=z();
}
//////////////////////////////////////////////////////////////////////////////
void Surface::receive(Light& l)
{
    assert(_sType!="");
    if( (_sType=="reflect") || (_sType=="perfect_mirror") )
        reflect(l);
    else if (_sType=="stop")
        stop(l);
    else if (_sType=="image")
        stop(l);
    else if (_sType=="void")
        ; // surface does nothing
    else //all glasses or "prefect_lens"
        transmit(l);
}
//////////////////////////////////////////////////////////////////////////////
// stop photons
void Surface::stop(Light& l)
{
    for (int i=0;i<l.nb_photon();i++)
    {
        Photon& p=l.get_photon(i);

        if (!p.is_valid())
            continue;

        local_ref(p);

        stop_photon(p);
        global_ref(p);

        if (!p.is_valid())
            continue;
    }
}
///////////////////////////////////////////////////////////////////////////////
// reflect photons
void Surface::reflect(Light& l)
{
    for (int i=0;i<l.nb_photon();i++)
    {
        Photon& p=l.get_photon(i);

        if (!p.is_valid())
            continue;

        local_ref(p);

        reflect_photon(p);
        global_ref(p);
    }
}
///////////////////////////////////////////////////////////////////////////////
// transmit photons
void Surface::transmit(Light& l)
{
    _pMaterialPrev=&(l.material());
    _pMaterialNext=_pMaterial;

    assert(_pMaterialPrev);

    for (int i=0;i<l.nb_photon();i++)
    {
        Photon& p=l.get_photon(i);

        if (!p.is_valid())
            continue;

        local_ref(p);

        transmit_photon(p);
        global_ref(p);
    }

    if( (_pMaterial!=0)  && (_bIsPerfect==false))
        l.set_material(_pMaterial);
}
//////////////////////////////////////////////////////////////////////////////
void Surface::transmit_photon(Photon& p)
{
    if (!p.is_valid())
        return;

    stop_photon(p);
    if (!p.is_valid())
        return;

    if(_bIsPerfect)
    {
        //todo optimise and merge tests
        if(p.dz==0.) //no intersection
        {
            p.valid=false;
            return;
        }

        //compute AP
        if( (_dCurvature<1.e-20) && (_dCurvature>-1.e-20))
            return; //flat lens

        double dFocal=0.5/_dCurvature;
        double t=dFocal/p.dz;

        if(p.dz<0.)
            t=-t;

        double ax=p.dx*t;
        double ay=p.dy*t;
        double az=p.dz*t;

        p.dx=ax-p.x;
        p.dy=ay-p.y;
        p.dz=az-p.z;

        if(dFocal<0.)
        {
            p.dx=-p.dx;
            p.dy=-p.dy;
            p.dz=-p.dz;
        }

        return;
    }

    if( (_pMaterialPrev==0) || (_pMaterialNext==0))
        return; //rien a faire car on ne connait pas les Materials

    assert(_pMaterialNext!=0);
    assert(_pMaterialPrev!=0);

    double nx,ny,nz;

    //calcule la normale en ce point ( de distance au centre: ro)
    double dRadiusSq=sqr(p.x)+sqr(p.y);
    if (dRadiusSq!=0.)
    {
        compute_normal(p.x,p.y,p.z,nx,ny,nz);
        Vector3D::normalize(nx,ny,nz);
    }
    else // au centre du miroir ou surface plate
    {
        nx=0.;
        ny=0.;
        nz=1.;
    }

    double u=nx*p.dx+ny*p.dy+nz*p.dz;

    // a partir d'ici, on calcule avec la normale (dx,dy,dz)
    // u est la projection de I sur la normale

    //calcule de C1=u/I
    double dC12=u*u/(sqr(p.dx)+sqr(p.dy)+sqr(p.dz));
    double dRatioN=_pMaterialNext->index(p.lambda())/_pMaterialPrev->index(p.lambda()); //TODO store index and reuse
    double denom=sqr(dRatioN)+dC12-1.;

    if (denom<=0.)
    {  // reflexion totale
        p.valid=false;
        return;
    }

    double dT2T1=sqrt(dC12/denom);
    double k=(1.-dT2T1)*u;

    p.dx=nx*k+dT2T1*p.dx;
    p.dy=ny*k+dT2T1*p.dy;
    p.dz=nz*k+dT2T1*p.dz;
}
//////////////////////////////////////////////////////////////////////////////
void Surface::reflect_photon(Photon &p)
{
    if (!p.is_valid())
        return;

    stop_photon(p);
    if (!p.is_valid())
        return;

    if(_bIsPerfect)
    {
        //todo optimise and merge tests
        if(p.dz==0.) //no intersection
        {
            p.valid=false;
            return;
        }

        //compute AP
        if( (_dCurvature<1.e-20) && (_dCurvature>-1.e-20))
        {
            p.dz=-p.dz;
            return; //flat mirror
        }

        double dFocal=0.5/_dCurvature;
        double t=dFocal/p.dz;

        if(p.dz<0.)
            t=-t;

        double ax=p.dx*t;
        double ay=p.dy*t;
        double az=p.dz*t;

        p.dx=ax-p.x;
        p.dy=ay-p.y;
        p.dz=az-p.z;

        if(dFocal<0.)
        {
            p.dx=-p.dx;
            p.dy=-p.dy;
            p.dz=-p.dz;
        }

        p.dz=-p.dz; //perfect mirror from perfect lens
        return;
    }

    double nx,ny,nz;
    compute_normal(p.x,p.y,p.z,nx,ny,nz);

    // on calcule avec la normale (nx,ny,nz)
    double dRayonSq=sqr(nx)+sqr(ny)+sqr(nz);
    assert(dRayonSq>0.);
    double u=2.*(nx*p.dx+ny*p.dy+nz*p.dz)/dRayonSq;
    p.dx-=nx*u;
    p.dy-=ny*u;
    p.dz-=nz*u;
}
///////////////////////////////////////////////////////////////////////////////
void Surface::set_comment(string sComment)
{
    _sComment=sComment;
}
///////////////////////////////////////////////////////////////////////////////
string Surface::comment() const
{
    return _sComment;
}
///////////////////////////////////////////////////////////////////////////////
void Surface::set_autofocus(bool bAutoFocus)
{
    _bAutoFocus= bAutoFocus;
}
///////////////////////////////////////////////////////////////////////////////
bool Surface::get_autofocus() const
{
    return _bAutoFocus;
}
///////////////////////////////////////////////////////////////////////////////
void Surface::set_conic(double dConic)
{
    _dConic=dConic;
    update_geometry();
}
///////////////////////////////////////////////////////////////////////////////
double Surface::conic() const
{
    return _dConic;
}
///////////////////////////////////////////////////////////////////////////////
void Surface::update_geometry()
{
    _bIsFlat=false;
    _bIsSpherical=false;
    _bIsConic=false;

    _bIsAspheric=( (_dR4!=0.) || (_dR6!=0.) || (_dR8!=0.) || (_dR10!=0.) );

    if(_bIsAspheric)
        return;

    if(abs(_dCurvature)<=1.e-20)
    {
        _bIsFlat=true;
        return;
    }

    if(_dConic!=0.)
        _bIsConic=true;
    else
        _bIsSpherical=true;
}
//////////////////////////////////////////////////////////////////////////////
bool Surface::is_aspheric() const
{
    return _bIsAspheric;
}
///////////////////////////////////////////////////////////////////////////////
void Surface::stop_photon(Photon& p)
{
    if (p.is_valid()==false)
        return;

    if(isinf(_dCurvature))
    {
        p.valid=false;
        return;
    }

    // first we go on flat case (intersect with z=0)
    if (p.dz==0.)
    {
        //cas degenere TODO
        p.valid=false; // pour l'instant, je ne sait que faire!
        return;
    }

    double tmin=-p.z/p.dz;
    if(_bIsFlat && (tmin<0.) )
    {
        // in the past of the photon
        p.valid=false;
        return;
    }

    // TODO optimise in case of spherical

    // stop the photon in z=0
    p.x+=tmin*p.dx;
    p.y+=tmin*p.dy;
    p.z=0.;

    if(_bIsFlat || _bIsPerfect)
    {
        p.valid=verify_in_surface(p.x,p.y);
        return;
    }

    //on calcule les coef de l'eq du 2eme degree en t:
    //the equation is t^2A+tB+C=0

    // formules simplifiees avec p.z=0.
    double dA=_dCurvature*(sqr(p.dx)+sqr(p.dy)+(_dConic+1.)*sqr(p.dz));
    double dB=2.*(_dCurvature*(p.x*p.dx+p.y*p.dy)-p.dz);
    double dC=_dCurvature*(sqr(p.x)+sqr(p.y));

    double tfinal;

    if (dA==0.)
    {
        if (dB==0.)
        {
            p.valid=false;
            return;
        }

        //the equation is now : t*dB+dC=0 so:
        tfinal=-dC/dB;
    }
    else //dA!=0
    {
        //on resoud l'equation
        double dB2=dB*dB;
        double delta=dB2-4.*dC*dA;

        if (delta<0.)
        {
            p.valid=false;
            return;
        }

        double t1,t2;

        if (delta!=dB2) // alors les denum existent (test a parfaire)
        {
            double sqrtDelta=sqrt(delta);
            t1=(2.*dC)/(+sqrtDelta-dB);
            t2=(2.*dC)/(-sqrtDelta-dB);
        }
        else
        {
            // ici delta~=dB2
            // on utilise une solution approchee:
            if (dB!=0)
            {
                t1=-dC/dB;
                t2=10.*t1; // pour etre sur de choisir t1
            }
            else
            {
                p.valid=false;  // on ne devrait jamais passer ici !??!?
                return;
            }
        }

        // on prend le t qui donne le z le plus proche de zero
        //t1ok=abs(z+t1.*dz)<abs(z+t2.*dz); //oct ver
        if (t1*t1<t2*t2) //a optimiser plus tard...
            tfinal=t1;
        else
            tfinal=t2;
    }

    //verifie que la surface est bien dans le futur du photon
    if ( tmin+tfinal<0 )
    {
        p.valid=false;
        return;
    }

    p.x+=p.dx*tfinal;
    p.y+=p.dy*tfinal;
    p.z+=p.dz*tfinal;

    assert(p.is_valid());

    p.valid=verify_in_surface(p.x,p.y);

    if(!_bIsAspheric)
        return;

    //aspheric mode
    //p.x,p.y,p.z is already a good approximation of the surface (as a conic), but make some newton step

    double x=p.x;
    double y=p.y;
    double z=p.z;
    double dOldT=0;
    for(int iLoop=0;iLoop<NB_ITER_STOP_NEWTON;iLoop++)
    {
        //reproject z on aspheric curve
        double zproj;
        compute_z(x,y,zproj);

        //compute normal on aspheric surface
        double nx,ny,nz;
        compute_normal(x,y,zproj,nx,ny,nz);

        //compute the d value to have the plane x*nx+y*ny+z*nz+d=0
        double d=-(x*nx+y*ny+zproj*nz);

        //compute the intersect of this plane and the line defined by p (one newton step)
        double t=(-d-p.x*nx-p.y*ny-p.z*nz)/(p.dx*nx+p.dy*ny+p.dz*nz); //todo tester !=0
        double distSQ=sqr(t-dOldT)*(sqr(p.dx)+sqr(p.dy)+sqr(p.dz));
        x=p.x+t*p.dx;
        y=p.y+t*p.dy;
        z=p.z+t*p.dz;

        if(distSQ<sqr(RESOLUTION_STOP_NEWTON))
        {
            p.x=x;
            p.y=y;
            p.z=z;

            p.valid=verify_in_surface(p.x,p.y);
            return;
        }

        dOldT=t;
    }

    //too many iterations
    p.valid=false;
    return;
}
//////////////////////////////////////////////////////////////////////////////
bool Surface::compute_normal(double x,double y, double z,double& nx,double& ny,double &nz)
{
    if(_bIsFlat)
    {
        nx=0.;
        ny=0.;
        nz=1.;
        return true;
    }

    if(_bIsSpherical)
    {
        assert(x*x+y*y<=1./sqr(_dCurvature)); //check the point is on the surface

        // optimisation in case of spherical shape
        // the normal target the center sphere at (0,0,1/curv) from (x,y,z)
        // so it is the vector (0-x,0-y 1/curv-z)
        // we multiply by curv to avoid the division

        nx=-x*_dCurvature;
        ny=-y*_dCurvature;
        nz=1.-z*_dCurvature;
        return true;
    }

    //the distance from revolution center is h=sqrt(x*x+y*y)
    // so square(distance) is h2=x*x+y*y
    double h2=sqr(x)+sqr(y);

    // here, we compute the tangent vector, multiplied by radius h; this is ok since we are only interested by its direction
    // and not its norm. This way the computation take one sqrt less:
    // at first, the tangent vector is [ x/h, y/h, dz/dh]
    // if me multiply by h : its became [ x, y, h*dz/dh ] wich is easier to compute due to the dh in the
    // formula of dz/dh, this give h*dz/dh -> h2*dw/dh and dw is simpler

    if (h2>0.)
    {
        double dTangRadius;
        if(!_bIsAspheric)
        {
            //conic case
            //dTangRadius est la pente par rapport au rayon (multiplied by radius h)
            // there is a trick in the conic case, we reuse z to avoid computing the sqrt again
            dTangRadius=z*(2.+sqr(z)*(_dConic+1.)*_dCurvature/(_dCurvature*h2-z));
        }
        else
        {
            //aspheric case

            //alternative method with one sqrt
            double dA=_dCurvature;
            double dB=sqr(_dCurvature)*(_dConic+1.);
            double dR=sqrt(1.-dB*h2);

            // real conic derivative versus radius (multiplied by radius h)
            dTangRadius=h2*(2.*dA/(dR+1.)+dA*dB*h2/dR/sqr((dR+1.))); //todo optimise

            // add the polynomial derivate (multiplied by radius h)
            double h4=sqr(h2);
            double h8=sqr(h4);
            dTangRadius+=4.*_dR4*h4+6.*_dR6*h4*h2+8.*_dR8*h8+10.*_dR10*h8*h2;  //todo use horner rule
        }

        // le vecteur tangent is parallel a la circonference
        // produit vectoriel pour calculer la normale
        nx=-x*dTangRadius;
        ny=-y*dTangRadius;
        nz=h2;
    }
    else
    {
        nx=0.;
        ny=0.;
        nz=1.;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////
bool Surface::compute_z(double x, double y, double &z)
{
    if(_bIsFlat || _bIsPerfect)
    {
        z=0.;
        return true;
    }

    double h2=sqr(x)+sqr(y);
    double dA=1.-(_dConic+1.)*h2*sqr(_dCurvature);
    if(dA<0.)
        return false;

    z= (h2*_dCurvature)/(1.+sqrt(dA));
    assert(!isnan(z));

    if(_bIsAspheric)
    {
        double h4=sqr(h2);
        double h6=h4*h2;
        z+=_dR4*h4+_dR6*h6+_dR8*sqr(h4)+_dR10*h4*h6; //TODO use horner form
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_radius_curvature(double dRadiusCurvature)
{
    _dCurvature=1./dRadiusCurvature;
    update_geometry();
}
//////////////////////////////////////////////////////////////////////////////
double Surface::radius_curvature() const
{
    return 1./_dCurvature;
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_R4(double dR4)
{
    _dR4=dR4;
    update_geometry();
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_R6(double dR6)
{
    _dR6=dR6;
    update_geometry();
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_R8(double dR8)
{
    _dR8=dR8;
    update_geometry();
}
//////////////////////////////////////////////////////////////////////////////
void Surface::set_R10(double dR10)
{
    _dR10=dR10;
    update_geometry();
}
//////////////////////////////////////////////////////////////////////////////
double Surface::R4() const
{
    return _dR4;
}
//////////////////////////////////////////////////////////////////////////////
double Surface::R6() const
{
    return _dR6;
}
//////////////////////////////////////////////////////////////////////////////
double Surface::R8() const
{
    return _dR8;
}
//////////////////////////////////////////////////////////////////////////////
double Surface::R10() const
{
    return _dR10;
}
//////////////////////////////////////////////////////////////////////////////
