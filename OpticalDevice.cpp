// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <cassert>
#include <cmath>

#include "OpticalDevice.h"
#include "Light.h"
#include "LightAutofocus.h"

//////////////////////////////////////////////////////////////////////////////
OpticalDevice::OpticalDevice()
{
    _iGridX=35;
    _iGridY=35;
    _iNbAngles=3;

    _dHalfFov=0.;
    _bMustRetrace=false;

    set_convention("relative");
}
//////////////////////////////////////////////////////////////////////////////
OpticalDevice::OpticalDevice(const OpticalDevice &rDevice)
{
    operator=(rDevice);
}
//////////////////////////////////////////////////////////////////////////////
OpticalDevice& OpticalDevice::operator=(const OpticalDevice& rDevice)
{
    _vdRelativeTick=rDevice._vdRelativeTick; //todo remove

    _theSurf.clear();
    _theSurf=rDevice._theSurf;
    /*
    for(int i=0;i<rDevice.nb_surface();i++)
    {
        insert_surface(i);
        *_theSurf[i]=*rDevice._theSurf[i]; //todo
    }
*/
    set_convention(rDevice.convention());
    set_note(rDevice.note());

    set_half_field_of_view(rDevice.half_field_of_view());

    _iGridX=rDevice._iGridX;
    _iGridY=rDevice._iGridY;

    set_nb_intermediate_angles(rDevice.nb_intermediate_angles());
    set_light_colors(rDevice.light_colors());

    _bMustRetrace=true;
    //_imageQuality=rDevice._imageQuality;

    return *this;
}
//////////////////////////////////////////////////////////////////////////////
OpticalDevice::~OpticalDevice()
{
    /*
    for (unsigned int iPos=0;iPos<_theSurf.size();iPos++)
    {
        delete _theSurf[iPos];
    }
    */
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::insert_surface(int iPos)
{
    _vdRelativeTick.insert(_vdRelativeTick.begin()+iPos,0);

    Surface surf;
  //  surf.set_diameter(0.);
  //  surf.set_conic(0.);
  //  surf.set_radius_curvature(RADIUS_CURVATURE_INFINITY);
 //   surf.set_z(0.);
 //   surf.set_type("void");

    _theSurf.insert(_theSurf.begin()+iPos,surf);

    update_absolute_surfaces();
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::delete_surface(int iPos)
{
    _vdRelativeTick.erase(_vdRelativeTick.begin()+iPos);

    assert(iPos>=0);
    assert(iPos<(int)_theSurf.size());

    _theSurf.erase(_theSurf.begin()+iPos);

    update_absolute_surfaces();
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_convention(string sConvention)
{
    if(_sConvention==sConvention)
        return;

    _sConvention=sConvention;
    _bIsRelativeSurface=(_sConvention=="relative");

    update_relative_surfaces();
}
//////////////////////////////////////////////////////////////////////////////
string OpticalDevice::convention() const
{
    return _sConvention;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::update_relative_surfaces()
{
    if(!_bIsRelativeSurface)
    {
        //absolute
        for(int i=0;i<nb_surface();i++)
        {
            _vdRelativeTick[i]=_theSurf[i].z();
        }

        return;
    }
    else
    {
        //relative
        for(int i=0;i<nb_surface()-1;i++)
        {
            double z1=_theSurf[i].z();
            double z2=_theSurf[i+1].z();
            _vdRelativeTick[i]=z2-z1;
        }

        if(nb_surface()!=0)
            _vdRelativeTick[nb_surface()-1]=0;
    }
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::update_absolute_surfaces()
{
    if(!_bIsRelativeSurface)
    {
        for(int i=0;i<nb_surface();i++)
        {
            _theSurf[i].set_z(_vdRelativeTick[i]);
        }

        return;
    }

    double dStart=0.;
    for(int i=0;i<nb_surface();i++)
    {
        _theSurf[i].set_z(dStart);
        dStart+=_vdRelativeTick[i];
    }
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_note(string sNote)
{
    _sNote=sNote;
}
//////////////////////////////////////////////////////////////////////////////
string OpticalDevice::note() const
{
    return _sNote;
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::has_aspheric() const
{
    for(int i=0;i<nb_surface();i++)
    {
        if(_theSurf[i].is_aspheric())
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::has_inner_diameter() const
{
    for(int i=0;i<nb_surface();i++)
    {
        if(_theSurf[i].inner_diameter()!=0.)
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_z(int iSurface,double dVal)
{
    _vdRelativeTick[iSurface]=dVal;
    update_absolute_surfaces();
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
double OpticalDevice::z(int iSurface)
{
    ray_trace();
    return  _vdRelativeTick[iSurface];
}
//////////////////////////////////////////////////////////////////////////////
double OpticalDevice::global_z(int iSurface)
{
    ray_trace();
    return _theSurf[iSurface].z();
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_autofocus(int iSurface,bool bAutofocus)
{
    if(bAutofocus && _theSurf[iSurface].type()!="image")
        return;

    _theSurf[iSurface].set_autofocus(bAutofocus);
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::get_autofocus(int iSurface)
{
    return _theSurf[iSurface].get_autofocus();
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_comment(int iSurface,string sComment)
{
    _theSurf[iSurface].set_comment(sComment);
}
//////////////////////////////////////////////////////////////////////////////
string OpticalDevice::comment(int iSurface) const
{
    return _theSurf[iSurface].comment();
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::has_comment() const
{
    for(int i=0;i<nb_surface();i++)
    {
        if(_theSurf[i].comment().empty()==false)
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////
int OpticalDevice::nb_surface() const
{
    return _theSurf.size();
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::ray_trace()
{
    if(!_bMustRetrace)
        return;

    _imageQuality.init(_iNbAngles);

    if(nb_surface()==0)
        return;

    // reset autodiameter
    for (int i=0;i<nb_surface();i++)
    {
        _theSurf[i].set_auto_diameter(_theSurf[i].get_auto_diameter());
        _theSurf[i].set_auto_inner_diameter(_theSurf[i].get_auto_inner_diameter());
    }

    //init autofocus
    Surface& pSurf=_theSurf[nb_surface()-1];
    Light light;

    bool bAutoFocus=(pSurf.type()=="image") && pSurf.get_autofocus();
    if(bAutoFocus) //TODO move in main loop
    {
        // init z pos with autofocus on axis
        initialise_light(&light,0,_iGridX,_iGridY);
        for (int i=0;i<nb_surface()-1;i++)
        {
            _theSurf[i].receive(light);
        }

        //compute AF on light
        LightAutofocus la;
        double dZ=la.autofocus(light);
        _theSurf[nb_surface()-1].set_z(dZ);

        update_relative_surfaces();
    }

    //todo re use last computation at tilt=0 if AF

    for(int iStep=0;iStep<_iNbAngles;iStep++)
    {
        double dTilt=0.;
        if(_iNbAngles>1)
            dTilt=half_field_of_view()*iStep/(double)(_iNbAngles-1);

        _imageQuality.vdAngles[iStep]=dTilt;

        initialise_light(&light,dTilt,_iGridX,_iGridY);
        for (int i=0;i<nb_surface();i++)
        {
            _theSurf[i].receive(light);
            if(i==0)
                light.init_vignetting();
        }

        light.compute_spot_size();

        if(iStep==0)
        {
            _imageQuality.dFNumber=light.get_FD();
            _imageQuality.dAirySize=light.airy_radius()*2;
        }

        //get quality result
        double dCenterX,dCenterY;
        light.get_spot_center(dCenterX,dCenterY);
        dCenterX=-dCenterX;
        if(fabs(dCenterX)<1.e-8)
            dCenterX=0;

        _imageQuality.vdDist[iStep]=dCenterX;
        _imageQuality.vdVignetting[iStep]=light.vignetting();
        _imageQuality.vdSpotSize[iStep]=light.spot_size();
        _imageQuality.vdSpotvsAiry[iStep]=light.spot_vs_airy();
    }

    _bMustRetrace=false;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_light_grid(int iGridX,int iGridY)
{
    if( (iGridX==_iGridX) && (iGridY==_iGridY) )
        return;

    _iGridX=iGridX;
    _iGridY=iGridY;

    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::compute_light(Light* pLight,int iSurface,double dTilt,int iGridX,int iGridY)
{
    ray_trace(); // for auto diameter and autofocus
    initialise_light(pLight,dTilt,iGridX,iGridY);

    for (int i=0;i<=iSurface;i++)
    {
        _theSurf[i].receive(*pLight);
        if(i==0)
            pLight->init_vignetting();
    }

    //finish computing quality
    pLight->compute_spot_size();
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::compute_surface_profile(int iSurface,double dX,double dY,double& dZ)
{
    ray_trace(); //for the auto diameter
    return _theSurf[iSurface].compute_z(dX,dY,dZ);
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::initialise_light(Light* pLight,double dTilt,int iGridX,int iGridY)
{
    assert(pLight);

    if(_theSurf.empty())
        return;

    Surface& pSurf=_theSurf[0];

    pLight->set_nb_photons(iGridX,iGridY);
    pLight->set_tilt(dTilt,0);
    pLight->set_colors(_sLightColors);

    double dDecal;
    pSurf.compute_z(0,pSurf.diameter()/2.,dDecal);
    pLight->set_geometry(pSurf.z()+dDecal,pSurf.diameter());
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_light_colors(const string & sLightColors)
{
    if(sLightColors==_sLightColors)
        return;

    _sLightColors=sLightColors;
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
string OpticalDevice::light_colors() const
{
    return _sLightColors;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_half_field_of_view(double dHalfFov)
{
    if(dHalfFov==_dHalfFov)
        return;

    _dHalfFov=dHalfFov;
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
double OpticalDevice::half_field_of_view() const
{
    return _dHalfFov;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_type(int iSurf,string sType)
{
    _theSurf[iSurf].set_type(sType);
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
string OpticalDevice::type(int iSurf) const
{
    return _theSurf[iSurf].type();
}
//////////////////////////////////////////////////////////////////////////////
const ImageQuality* OpticalDevice::get_image_quality()
{
    ray_trace();
    return &_imageQuality; // todo copy
}
//////////////////////////////////////////////////////////////////////////////
int OpticalDevice::nb_intermediate_angles() const
{
    return _iNbAngles;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_nb_intermediate_angles(int iNbAngles)
{
    if(iNbAngles==_iNbAngles)
        return;

    if(iNbAngles<1)
        iNbAngles=1;

    _iNbAngles=iNbAngles;
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
double OpticalDevice::get(int iSurface,eSurfaceparameter eParam)
{
    Surface& r=_theSurf[iSurface];

    if(eParam==RADIUS_CURVATURE)
        return r.radius_curvature();

    if(eParam==CONIC)
        return r.conic();

    if(eParam==R4)
        return r.R4();

    if(eParam==R6)
        return r.R6();

    if(eParam==R8)
        return r.R8();

    if(eParam==R10)
        return r.R10();

    if(eParam==AUTO_DIAMETER)
        return r.get_auto_diameter();

    if(eParam==AUTO_INNER_DIAMETER)
        return r.get_auto_inner_diameter();

    ray_trace(); //parameters below need full raytrace

    if(eParam==DIAMETER)
        return r.diameter();

    if(eParam==INNER_DIAMETER)
        return r.inner_diameter();

    return -1;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set(int iSurface,eSurfaceparameter eParam,double dParam)
{
    Surface& r=_theSurf[iSurface];

    if(eParam==DIAMETER)
        r.set_diameter(dParam);

    if(eParam==AUTO_DIAMETER)
        r.set_auto_diameter(dParam);

    if(eParam==INNER_DIAMETER)
        r.set_inner_diameter(dParam);

    if(eParam==AUTO_INNER_DIAMETER)
        r.set_auto_inner_diameter(dParam);

    if(eParam==RADIUS_CURVATURE)
        r.set_radius_curvature(dParam);

    if(eParam==CONIC)
        r.set_conic(dParam);

    if(eParam==R4)
        r.set_R4(dParam);

    if(eParam==R6)
        r.set_R6(dParam);

    if(eParam==R8)
        r.set_R8(dParam);

    if(eParam==R10)
        r.set_R10(dParam);

    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
