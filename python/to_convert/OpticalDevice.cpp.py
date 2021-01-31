# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <cassert>
#include <cmath>
#include <sstream>

#include "OpticalDevice.h"
#include "Light.h"
#include "LightAutofocus.h"

#######################################
OpticalDevice.OpticalDevice()
    _iGridX= NB_PHOTON_X
    _iGridY= NB_PHOTON_Y
    _iNbAngles=3

    _dHalfFov=0.
    _bMustRetrace=False

    _bAutoCurvature=False
    _bAutoFocus=False

    set_relative_convention(True)

#######################################
OpticalDevice.OpticalDevice( OpticalDevice &rDevice)
    operator=(rDevice)

#######################################
OpticalDevice& OpticalDevice.operator=( OpticalDevice& rDevice)
    _vdThicks=rDevice._vdThicks; #TODO remove

    _vSurfaces.clear()
    _vSurfaces=rDevice._vSurfaces

    set_relative_convention(rDevice.relative_convention())
    set_note(rDevice.note())

    set_half_field_of_view(rDevice.half_field_of_view())

    _iGridX=rDevice._iGridX
    _iGridY=rDevice._iGridY

    set_nb_intermediate_angles(rDevice.nb_intermediate_angles())
    set_light_colors(rDevice.light_colors())

    _bAutoCurvature=rDevice._bAutoCurvature
    _bAutoFocus=rDevice._bAutoFocus

    _bMustRetrace=True

    return *self

#######################################
OpticalDevice.~OpticalDevice()
{
#######################################
def insert_surface(self, iPos):
    assert(iPos>=0)
    assert(iPos<=nb_surface())

    _vSurfaces.insert(_vSurfaces.begin()+iPos,Surface())
    _vdThicks.insert(_vdThicks.begin()+iPos,0)

    _vdThicks[iPos]=0

    if(nb_surface()==1) #first time
        _vSurfaces[0].set_z(0)
        return


    if iPos<nb_surface()-1:
        #intermediate position
        _vSurfaces[iPos].set_z(_vSurfaces[iPos+1].z())

    else:
        #last position
        _vSurfaces[iPos].set_z(_vSurfaces[iPos-1].z()+_vdThicks[iPos-1])


    # update clone
    for(unsigned int i=0;i<_surfParamsClone.size();i++)
        # if clone gap increased, ref
        if _surfParamsClone[i].iRefSurface>=iPos:
            _surfParamsClone[i].iRefSurface++
        if _surfParamsClone[i].iSurface>=iPos:
            _surfParamsClone[i].iSurface++


#######################################
def delete_surface(self, iPos):
    assert(iPos>=0)
    assert(iPos<(int)_vSurfaces.size())

    _vdThicks.erase(_vdThicks.begin()+iPos)
    _vSurfaces.erase(_vSurfaces.begin()+iPos)

    update_thicks()
    _bMustRetrace=True

    # update clone
    unsigned int i=0
    while(i<_surfParamsClone.size())
        if((_surfParamsClone[i].iSurface==iPos) or (_surfParamsClone[i].iRefSurface==iPos) ) #link broken, remove
            _surfParamsClone.erase(_surfParamsClone.begin()+i)

        else:
            # if clone removed, ref or orig
            if _surfParamsClone[i].iRefSurface>iPos:
                _surfParamsClone[i].iRefSurface--
            if _surfParamsClone[i].iSurface>iPos:
                _surfParamsClone[i].iSurface--
            i++



#######################################
def set_relative_convention(self, bRelativeConvention):
    _bRelativeConvention=bRelativeConvention

#######################################
def relative_convention(self):
    return _bRelativeConvention

#######################################
def update_thicks(self):
    if nb_surface()==0:
        return

    for(int i=0;i<nb_surface()-1;i++)
        double z1=_vSurfaces[i].z()
        double z2=_vSurfaces[i+1].z()
        _vdThicks[i]=z2-z1


#######################################
def update_z(self):
    if nb_surface()==0:
        return

    double dZ=_vSurfaces[0].z()

    for(int i=0;i<nb_surface();i++)
        _vSurfaces[i].set_z(dZ)
        dZ+=_vdThicks[i]


#######################################
def set_note(self, sNote):
    _sNote=sNote

#######################################
def note(self):
    return _sNote

#######################################
def has_aspheric(self):
    for(int i=0;i<nb_surface();i++)
        if _vSurfaces[i].is_aspheric():
            return True


    return False

#######################################
def has_inner_diameter(self):
    for(int i=0;i<nb_surface();i++)
        if _vSurfaces[i].inner_diameter()!=0.:
            return True


    return False

#######################################
def has_auto(self):
    if _bAutoFocus:
        return True

    if _bAutoCurvature:
        return True

    for(int i=0;i<nb_surface();i++)
         Surface& s=_vSurfaces[i]
        bool bAuto=s.get_auto_diameter() or s.get_auto_inner_diameter()
        if bAuto:
            return True


    return False

#######################################
def set_autofocus(self, bAutofocus):
    _bMustRetrace|=(not _bAutoFocus) and bAutofocus
    _bAutoFocus=bAutofocus

#######################################
def get_autofocus(self):
    return _bAutoFocus

#######################################
def set_image_autocurvature(self, bAutoCurvature):
    _bMustRetrace|= (not _bAutoCurvature) and bAutoCurvature
    _bAutoCurvature=bAutoCurvature

#######################################
def get_image_autocurvature(self):
    return _bAutoCurvature

#######################################
def set_comment(self, iSurface, sComment):
    _vSurfaces[iSurface].set_comment(sComment)

#######################################
def comment(self, iSurface):
    return _vSurfaces[iSurface].comment()

#######################################
def has_comment(self):
    for(int i=0;i<nb_surface();i++)
        if _vSurfaces[i].comment().empty()==False:
            return True


    return False

#######################################
def nb_surface(self):
    return (int)_vSurfaces.size()

#######################################
def ray_trace(self):
    if not _bMustRetrace:
        return

    _imageQuality.init(_iNbAngles)

    if nb_surface()==0:
        return

    # reset autodiameter
    for (int i=0;i<nb_surface();i++)
        _vSurfaces[i].set_auto_diameter(_vSurfaces[i].get_auto_diameter())
        _vSurfaces[i].set_auto_inner_diameter(_vSurfaces[i].get_auto_inner_diameter())


    #update clone parameters
    for(unsigned int i=0;i<_surfParamsClone.size();i++)
        SurfaceParameterClone& a=_surfParamsClone[i]
        set(a.iSurface,a.param,a.dGain*get(a.iRefSurface,a.param,False))


    # main ray tracing loop on axis for autofocus
    Light light
    ray_trace_step(light,0,_bAutoFocus,False)
    _imageQuality.isImageInfinite=light.is_image_infinite()

    #get quality result
    double dCenterX,dCenterY
    light.get_spot_center(dCenterX,dCenterY)
    _imageQuality.vdAngles[0]=0
    _imageQuality.vdDist[0]=-dCenterX
    _imageQuality.vdVignetting[0]=light.vignetting()
    _imageQuality.vdSpotSize[0]=light.spot_size()
    _imageQuality.vdSpotvsAiry[0]=light.spot_vs_airy()

    #for autofocus and on-axis stats
    _imageQuality.dFNumber=light.get_FD()
    _imageQuality.dAirySize=light.airy_radius()*2.

    if(_iNbAngles>1) # for autocurvature
        ray_trace_step(light,half_field_of_view(),False,_bAutoCurvature); #for autocurvature

        #get quality result
        light.get_spot_center(dCenterX,dCenterY)
        _imageQuality.vdAngles[_iNbAngles-1]=half_field_of_view()
        _imageQuality.vdDist[_iNbAngles-1]=-dCenterX
        _imageQuality.vdVignetting[_iNbAngles-1]=light.vignetting()
        _imageQuality.vdSpotSize[_iNbAngles-1]=light.spot_size()
        _imageQuality.vdSpotvsAiry[_iNbAngles-1]=light.spot_vs_airy()
        _imageQuality.isImageInfinite=is_image_infinite()


    for(int iStep=1;iStep<_iNbAngles-1;iStep++)
        double dTilt=half_field_of_view()*iStep/(double)(_iNbAngles-1)
        _imageQuality.vdAngles[iStep]=dTilt

        ray_trace_step(light,dTilt,False,False)

        #get quality result
        light.get_spot_center(dCenterX,dCenterY)
        _imageQuality.vdDist[iStep]=-dCenterX
        _imageQuality.vdVignetting[iStep]=light.vignetting()
        _imageQuality.vdSpotSize[iStep]=light.spot_size()
        _imageQuality.vdSpotvsAiry[iStep]=light.spot_vs_airy()


    _bMustRetrace=False

#######################################
def ray_trace_step(self, light, dTilt, bAutofocus, bAutocurvature):
    initialize_light(&light,dTilt,_iGridX,_iGridY)
    for (int i=0;i<nb_surface()-1;i++)
        _vSurfaces[i].receive(light)
        if i==0:
            light.init_vignetting()


    # if autofocus, z before last receive
    if bAutofocus:
        #compute AF on light
        LightAutofocus la
        double dZ=la.autofocus(light)
        _vSurfaces[nb_surface()-1].set_z(dZ)
        update_thicks()


    if bAutocurvature:
        #compute AF on light
        LightAutofocus la
        double dZ=la.autofocus(light)

        double dCenterX,dCenterY
        la.get_center(dCenterX,dCenterY)

        # set curvature
        double dX1=dCenterX
        double dZ1=_vSurfaces[nb_surface()-1].z()-dZ
        double dR=-0.5*(dX1*dX1+dZ1*dZ1)/dZ1; # R =0.5*(x1*x1+z1*z1)/z1
        #TODO test dR
        _vSurfaces[nb_surface()-1].set_radius_curvature(dR)


    #finish receiving
    Surface& s=_vSurfaces[nb_surface()-1]
    s.receive(light)

    light.compute_spot_size(is_image_infinite())

#######################################
def set_light_grid(self, iGridX, iGridY):
    if  (iGridX==_iGridX) and (iGridY==_iGridY) :
        return

    _iGridX=iGridX
    _iGridY=iGridY

    _bMustRetrace=True

#######################################
def compute_light(self, pLight, iSurface, dTilt, iGridX, iGridY):
    ray_trace(); # for auto diameter and autofocus
    initialize_light(pLight,dTilt,iGridX,iGridY)

    for (int i=0;i<=iSurface;i++)
        _vSurfaces[i].receive(*pLight)
        if i==0:
            pLight.init_vignetting()


    #finish computing quality
    pLight.compute_spot_size(is_image_infinite())

#######################################
def compute_surface_profile(self, iSurface, dX, dY, dZ):
    ray_trace(); #for the auto parameters
    return _vSurfaces[iSurface].compute_z(dX,dY,dZ)

#######################################
def initialize_light(self, pLight, dTilt, iGridX, iGridY):
    assert(pLight)
	pSurf = 0

	# do not init on void surfaces
	for (i = 0; i <(int)_vSurfaces.size(); i++)
		if _vSurfaces[i].type() != "void":
			pSurf = &_vSurfaces[i]
			break



    if pSurf==0:
        return

    pLight.set_nb_photons(iGridX,iGridY)
    pLight.set_tilt(dTilt,0)
    pLight.set_colors(_sLightColors)

    double dDecal
    pSurf.compute_z(0,pSurf.diameter()/2.,dDecal)
    pLight.set_geometry(pSurf.z()+dDecal,pSurf.diameter())
    pLight.get_photon(0); # to force light.init ) TODO

#######################################
def set_light_colors(self, & sLightColors):
    if sLightColors==_sLightColors:
        return

    _sLightColors=sLightColors
    _bMustRetrace=True

#######################################
def light_colors(self):
    return _sLightColors

#######################################
def set_half_field_of_view(self, dHalfFov):
    if dHalfFov==_dHalfFov:
        return

    _dHalfFov=dHalfFov
    _bMustRetrace=True

#######################################
def half_field_of_view(self):
    return _dHalfFov

#######################################
def set_type(self, iSurf, sType):
    _vSurfaces[iSurf].set_type(sType)
    _bMustRetrace=True

#######################################
def type(self, iSurf):
    return _vSurfaces[iSurf].type()

#######################################
 ImageQuality OpticalDevice.get_image_quality()
    ray_trace()
    return _imageQuality; # TODO modify ref instead of copy

#######################################
def nb_intermediate_angles(self):
    return _iNbAngles

#######################################
def set_nb_intermediate_angles(self, iNbAngles):
    if iNbAngles==_iNbAngles:
        return

    if iNbAngles<1:
        iNbAngles=1

    _iNbAngles=iNbAngles
    _bMustRetrace=True

#######################################
def get(self, iSurface, eParam, bUpdate):
    if bUpdate:
        ray_trace()

     Surface& r=_vSurfaces[iSurface]

    if eParam==CONIC:
        return r.conic()

    if eParam==R4:
        return r.R4()

    if eParam==R6:
        return r.R6()

    if eParam==R8:
        return r.R8()

    if eParam==R10:
        return r.R10()

    if eParam==AUTO_DIAMETER:
        return r.get_auto_diameter()

    if eParam==AUTO_INNER_DIAMETER:
        return r.get_auto_inner_diameter()

    if eParam==Z:
        return r.z()

    if eParam==THICK:
        return _vdThicks[iSurface]

    if eParam==RADIUS_CURVATURE:
        return r.radius_curvature()

    if eParam==CURVATURE:
        return r.curvature()

    if eParam==DIAMETER:
        return r.diameter()

    if eParam==INNER_DIAMETER:
        return r.inner_diameter()

    return -1

#######################################
def set(self, iSurface, eParam, dParam):
    Surface& r=_vSurfaces[iSurface]

    if eParam==DIAMETER:
        r.set_diameter(dParam)

    if eParam==AUTO_DIAMETER:
        r.set_auto_diameter(dParam!=0.)

    if eParam==INNER_DIAMETER:
        r.set_inner_diameter(dParam)

    if eParam==AUTO_INNER_DIAMETER:
        r.set_auto_inner_diameter(dParam!=0.)

    if eParam==RADIUS_CURVATURE:
        r.set_radius_curvature(dParam)

    if eParam==CURVATURE:
        r.set_curvature(dParam)

    if eParam==CONIC:
        r.set_conic(dParam)

    if eParam==Z:
        r.set_z(dParam)
        update_thicks()


    if eParam==THICK:
        _vdThicks[iSurface]=dParam
        update_z()


    if eParam==R4:
        r.set_R4(dParam)

    if eParam==R6:
        r.set_R6(dParam)

    if eParam==R8:
        r.set_R8(dParam)

    if eParam==R10:
        r.set_R10(dParam)

    _bMustRetrace=True

#######################################
def set_parameter(self, & sKey, & sValue):
    _otherParameters[sKey]=sValue

#######################################
def set_parameter(self, & sKey, dValue):
    stringstream ss
    ss << dValue
    _otherParameters[sKey]=ss.str()

#######################################
def get_parameter(self, sKey, & dValue):
    iter = _otherParameters.find(sKey)
    if  iter==_otherParameters.end():
        return False

    stringstream ss(iter.second)
    ss >> dValue
    return True; #TODO test ss result

#######################################
def get_parameter(self, sKey, & sValue):
    iter = _otherParameters.find(sKey)
    if  iter==_otherParameters.end():
        return False

    sValue=iter.second
    return True

#######################################
 map<string, OpticalDevice.all_parameters()
    return _otherParameters

#######################################
# surface parameter cloning
def set_clone(self, iSurface, eParam, iRefSurface, dGain):
    eSurfaceParameter eParamAlias=eParam

    # Z and THICK are same
    if eParam==eSurfaceParameter.THICK:
        eParamAlias=eSurfaceParameter.Z
    if eParam==eSurfaceParameter.Z:
        eParamAlias=eSurfaceParameter.THICK

    if iRefSurface>=nb_surface():
        return

    if iRefSurface>=iSurface) #only backward cloning allowed (avoid loop:
        return

    for(unsigned int i=0;i<_surfParamsClone.size();i++)
        if  (_surfParamsClone[i].iSurface==iSurface) and ((_surfParamsClone[i].param==eParam)or(_surfParamsClone[i].param==eParamAlias)) :
            if iRefSurface>=0:
                # already existing, updating
                _surfParamsClone[i].iRefSurface=iRefSurface
                _surfParamsClone[i].dGain=dGain

            else:
                #remove clone
                _surfParamsClone.erase(_surfParamsClone.begin()+i)


            _bMustRetrace=True
            return


    if(iRefSurface<0) #nothing to remove
        return

    #create entry
    SurfaceParameterClone cloneSurf
    cloneSurf.iSurface=iSurface
    cloneSurf.param=eParam
    cloneSurf.iRefSurface=iRefSurface
    cloneSurf.dGain=dGain

    _surfParamsClone.push_back(cloneSurf)
    _bMustRetrace=True

#######################################
def get_clone(self, iSurface, eParam, iRefSurface, dGain):
    eSurfaceParameter eParamAlias=eParam

    # Z and THICK are same
    if eParam==eSurfaceParameter.THICK:
        eParamAlias=eSurfaceParameter.Z
    if eParam==eSurfaceParameter.Z:
        eParamAlias=eSurfaceParameter.THICK

    for(unsigned int i=0;i<_surfParamsClone.size();i++)
        if  (_surfParamsClone[i].iSurface==iSurface) and ((_surfParamsClone[i].param==eParam)or (_surfParamsClone[i].param==eParamAlias)) :
            iRefSurface=_surfParamsClone[i].iRefSurface
            dGain=_surfParamsClone[i].dGain
            return True


    return False

#######################################
def is_clone(self, iSurface, eParam):
    int iRefSurface
    double dGain
    return get_clone(iSurface,eParam,iRefSurface,dGain)

#######################################
def is_image_infinite(self):
    for(int i=0;i<nb_surface();i++)
        if _vSurfaces[i].type()=="image_infinite":
            return True


    return False

#######################################
