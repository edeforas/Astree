// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <cassert>
#include <cmath>
#include <sstream>

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

    _bAutoCurvature=false;
    _bAutoFocus=false;

    set_relative_convention(true);
}
//////////////////////////////////////////////////////////////////////////////
OpticalDevice::OpticalDevice(const OpticalDevice &rDevice)
{
    operator=(rDevice);
}
//////////////////////////////////////////////////////////////////////////////
OpticalDevice& OpticalDevice::operator=(const OpticalDevice& rDevice)
{
    _vdThicks=rDevice._vdThicks; //todo remove

    _vSurfaces.clear();
    _vSurfaces=rDevice._vSurfaces;

    set_relative_convention(rDevice.relative_convention());
    set_note(rDevice.note());

    set_half_field_of_view(rDevice.half_field_of_view());

    _iGridX=rDevice._iGridX;
    _iGridY=rDevice._iGridY;

    set_nb_intermediate_angles(rDevice.nb_intermediate_angles());
    set_light_colors(rDevice.light_colors());

    _bAutoCurvature=rDevice._bAutoCurvature;
    _bAutoFocus=rDevice._bAutoFocus;

    _bMustRetrace=true;

    return *this;
}
//////////////////////////////////////////////////////////////////////////////
OpticalDevice::~OpticalDevice()
{ }
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::insert_surface(int iPos)
{
    assert(iPos>=0);
    assert(iPos<=nb_surface());

    _vSurfaces.insert(_vSurfaces.begin()+iPos,Surface());
    _vdThicks.insert(_vdThicks.begin()+iPos,0);

    _vdThicks[iPos]=0;

    if(nb_surface()==1) //first time
    {
        _vSurfaces[0].set_z(0);
        return;
    }

    if(iPos<nb_surface()-1)
    {
        //intermediate position
        _vSurfaces[iPos].set_z(_vSurfaces[iPos+1].z());
    }
    else
    {
        //last position
        _vSurfaces[iPos].set_z(_vSurfaces[iPos-1].z()+_vdThicks[iPos-1]);
    }

    // update alias
    for(unsigned int i=0;i<_alias.size();i++)
    {
        // if alias to after removed, increment ref
        if(_alias[i].iRefSurface>=iPos)
            _alias[i].iRefSurface++;
    }
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::delete_surface(int iPos)
{
    assert(iPos>=0);
    assert(iPos<(int)_vSurfaces.size());

    _vdThicks.erase(_vdThicks.begin()+iPos);
    _vSurfaces.erase(_vSurfaces.begin()+iPos);

    update_thicks();
    _bMustRetrace=true;

    // update alias
    for(unsigned int i=0;i<_alias.size();i++)
    {
        if(_alias[i].iRefSurface==iPos)
        {
            _alias.erase(_alias.begin()+i);
        }
        else
        {
            // if alias to after removed, decrement ref
            if(_alias[i].iRefSurface>iPos)
                _alias[i].iRefSurface--;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_relative_convention(bool bRelativeConvention)
{
    _bRelativeConvention=bRelativeConvention;
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::relative_convention() const
{
    return _bRelativeConvention;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::update_thicks()
{
    if(nb_surface()==0)
        return;

    for(int i=0;i<nb_surface()-1;i++)
    {
        double z1=_vSurfaces[i].z();
        double z2=_vSurfaces[i+1].z();
        _vdThicks[i]=z2-z1;
    }
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::update_z()
{
    if(nb_surface()==0)
        return;

    double dZ=_vSurfaces[0].z();

    for(int i=0;i<nb_surface();i++)
    {
        _vSurfaces[i].set_z(dZ);
        dZ+=_vdThicks[i];
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
        if(_vSurfaces[i].is_aspheric())
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::has_inner_diameter() const
{
    for(int i=0;i<nb_surface();i++)
    {
        if(_vSurfaces[i].inner_diameter()!=0.)
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_autofocus(bool bAutofocus)
{
    _bAutoFocus=bAutofocus;
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::get_autofocus() const
{
    return _bAutoFocus;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_image_autocurvature(bool bAutoCurvature)
{
    _bAutoCurvature=bAutoCurvature;
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::get_image_autocurvature() const
{
    return _bAutoCurvature;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_comment(int iSurface,string sComment)
{
    _vSurfaces[iSurface].set_comment(sComment);
}
//////////////////////////////////////////////////////////////////////////////
string OpticalDevice::comment(int iSurface) const
{
    return _vSurfaces[iSurface].comment();
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::has_comment() const
{
    for(int i=0;i<nb_surface();i++)
    {
        if(_vSurfaces[i].comment().empty()==false)
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////
int OpticalDevice::nb_surface() const
{
    return (int)_vSurfaces.size();
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
        _vSurfaces[i].set_auto_diameter(_vSurfaces[i].get_auto_diameter());
        _vSurfaces[i].set_auto_inner_diameter(_vSurfaces[i].get_auto_inner_diameter());
    }

    //update alias parameters
    for(unsigned int i=0;i<_alias.size();i++)
    {
        Alias& a=_alias[i];
        set(a.iSurface,a.param,get(a.iRefSurface,a.param));
    }

    // main ray tracing loop
    Light light;
    ray_trace_step(light,0,_bAutoFocus,false);
    //get quality result
    double dCenterX,dCenterY;
    light.get_spot_center(dCenterX,dCenterY);
    _imageQuality.vdAngles[0]=0;
    _imageQuality.vdDist[0]=-dCenterX;
    _imageQuality.vdVignetting[0]=light.vignetting();
    _imageQuality.vdSpotSize[0]=light.spot_size();
    _imageQuality.vdSpotvsAiry[0]=light.spot_vs_airy();

    //for autofocus and on-axis stats
    _imageQuality.dFNumber=light.get_FD();
    _imageQuality.dAirySize=light.airy_radius()*2;

    if(_iNbAngles>1)
    {
        ray_trace_step(light,half_field_of_view(),false,_bAutoCurvature); //for autocurvature
        //get quality result
        double dCenterX,dCenterY;
        light.get_spot_center(dCenterX,dCenterY);
        _imageQuality.vdAngles[_iNbAngles-1]=half_field_of_view();
        _imageQuality.vdDist[_iNbAngles-1]=-dCenterX;
        _imageQuality.vdVignetting[_iNbAngles-1]=light.vignetting();
        _imageQuality.vdSpotSize[_iNbAngles-1]=light.spot_size();
        _imageQuality.vdSpotvsAiry[_iNbAngles-1]=light.spot_vs_airy();
    }

    for(int iStep=1;iStep<_iNbAngles-1;iStep++)
    {
        double dTilt=half_field_of_view()*iStep/(double)(_iNbAngles-1);
        _imageQuality.vdAngles[iStep]=dTilt;

        ray_trace_step(light,dTilt,false,false);

        //get quality result
        double dCenterX,dCenterY;
        light.get_spot_center(dCenterX,dCenterY);
        _imageQuality.vdDist[iStep]=-dCenterX;
        _imageQuality.vdVignetting[iStep]=light.vignetting();
        _imageQuality.vdSpotSize[iStep]=light.spot_size();
        _imageQuality.vdSpotvsAiry[iStep]=light.spot_vs_airy();
    }

    _bMustRetrace=false;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::ray_trace_step(Light& light,double dTilt,bool bAutofocus,bool bAutocurvature)
{
    initialize_light(&light,dTilt,_iGridX,_iGridY);
    for (int i=0;i<nb_surface()-1;i++)
    {
        _vSurfaces[i].receive(light);
        if(i==0)
            light.init_vignetting();
    }

    // if autofocus, compute z before last receive
    if(bAutofocus)
    {
        //compute AF on light
        LightAutofocus la;
        double dZ=la.autofocus(light);
        _vSurfaces[nb_surface()-1].set_z(dZ);
        update_thicks();
    }

    if(bAutocurvature)
    {
        //compute AF on light
        LightAutofocus la;
        double dZ=la.autofocus(light);

        double dCenterX,dCenterY;
        la.get_center(dCenterX,dCenterY);

        // set new curvature
        double dX1=dCenterX;
        double dZ1=_vSurfaces[nb_surface()-1].z()-dZ;
        double dR=-0.5*(dX1*dX1+dZ1*dZ1)/dZ1; // R =0.5*(x1*x1+z1*z1)/z1
        //todo test dR
        _vSurfaces[nb_surface()-1].set_radius_curvature(dR);
    }

    //finish receiving
    _vSurfaces[nb_surface()-1].receive(light);

    light.compute_spot_size();
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
    initialize_light(pLight,dTilt,iGridX,iGridY);

    for (int i=0;i<=iSurface;i++)
    {
        _vSurfaces[i].receive(*pLight);
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
    return _vSurfaces[iSurface].compute_z(dX,dY,dZ);
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::initialize_light(Light* pLight,double dTilt,int iGridX,int iGridY)
{
    assert(pLight);

    if(_vSurfaces.empty())
        return;

    Surface& pSurf=_vSurfaces[0];

    pLight->set_nb_photons(iGridX,iGridY);
    pLight->set_tilt(dTilt,0);
    pLight->set_colors(_sLightColors);

    double dDecal;
    pSurf.compute_z(0,pSurf.diameter()/2.,dDecal);
    pLight->set_geometry(pSurf.z()+dDecal,pSurf.diameter());
    pLight->get_photon(0); // to force light->init ) todo
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
    _vSurfaces[iSurf].set_type(sType);
    _bMustRetrace=true;
}
//////////////////////////////////////////////////////////////////////////////
string OpticalDevice::type(int iSurf) const
{
    return _vSurfaces[iSurf].type();
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
    Surface& r=_vSurfaces[iSurface];

    if(eParam==CONIC)
        return r.conic();

    if(eParam==RADIUS_CURVATURE)
        return r.radius_curvature();

    if(eParam==CURVATURE)
        return r.curvature();

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

    ray_trace(); //parameters below need full raytrace because of auto keyword

    if(eParam==Z)
        return r.z();

    if(eParam==THICK)
        return _vdThicks[iSurface];

    if(eParam==DIAMETER)
        return r.diameter();

    if(eParam==INNER_DIAMETER)
        return r.inner_diameter();

    return -1;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set(int iSurface,eSurfaceparameter eParam,double dParam)
{
    Surface& r=_vSurfaces[iSurface];

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

    if(eParam==CURVATURE)
        r.set_curvature(dParam);

    if(eParam==CONIC)
        r.set_conic(dParam);

    if(eParam==Z)
    {
        r.set_z(dParam);
        update_thicks();
    }

    if(eParam==THICK)
    {
        _vdThicks[iSurface]=dParam;
        update_z();
    }

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
void OpticalDevice::set_parameter(const string & sKey,const string & sValue)
{
    _otherParameters[sKey]=sValue;
}
//////////////////////////////////////////////////////////////////////////////
void OpticalDevice::set_parameter(const string & sKey,double dValue)
{
    stringstream ss;
    ss << dValue;
    _otherParameters[sKey]=ss.str();
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::get_parameter(const string& sKey,double & dValue) const
{
    auto iter= _otherParameters.find(sKey);
    if( iter==_otherParameters.end())
        return false;

    stringstream ss(iter->second);
    ss >> dValue;
    return true; //todo test ss result
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::get_parameter(const string& sKey,string & sValue) const
{
    auto iter= _otherParameters.find(sKey);
    if( iter==_otherParameters.end())
        return false;

    sValue=iter->second;
    return true;
}
//////////////////////////////////////////////////////////////////////////////
const map<string,string>& OpticalDevice::all_parameters() const
{
    return _otherParameters;
}
//////////////////////////////////////////////////////////////////////////////
// surface aliasing
void OpticalDevice::set_alias(int iSurface,eSurfaceparameter eParam,int iRefSurface)
{
    if(iRefSurface>=nb_surface())
        return;

    if(iRefSurface==iSurface)
        return;

    for(unsigned int i=0;i<_alias.size();i++)
        if( (_alias[i].iSurface==iSurface) && (_alias[i].param==eParam) )
        {
            if(iRefSurface>=0)
            {
                // already existing, updating
                _alias[i].iRefSurface=iRefSurface;
                set(iSurface,eParam,get(iRefSurface,eParam));
            }
            else
            {
                //remove alias
                _alias.erase(_alias.begin()+i);
            }
            return;
        }

    if(iRefSurface<0) //nothing to remove
        return;

    //create new entry
    Alias alias;
    alias.iSurface=iSurface;
    alias.param=eParam;
    alias.iRefSurface=iRefSurface;

    _alias.push_back(alias);

    set(iSurface,eParam,get(iRefSurface,eParam));
}
//////////////////////////////////////////////////////////////////////////////
bool OpticalDevice::get_alias(int iSurface,eSurfaceparameter eParam,int& iRefSurface)
{
    for(unsigned int i=0;i<_alias.size();i++)
        if( (_alias[i].iSurface==iSurface) && (_alias[i].param==eParam) )
        {
            iRefSurface=_alias[i].iRefSurface;
            return true;
        }

    return false;
}
//////////////////////////////////////////////////////////////////////////////
