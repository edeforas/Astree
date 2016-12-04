// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceOptimizer.h"
#include "OpticalDevice.h"

#include <cmath> //for isnan
#include <cassert>
////////////////////////////////////////////////////////////////////////////////
DeviceOptimizer::DeviceOptimizer():
    _pDevice(0),
    _meritFunction(eFullFrameMaxError)
{ }
////////////////////////////////////////////////////////////////////////////////
DeviceOptimizer::~DeviceOptimizer()
{ }
////////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::clear()
{
    _pDevice=0;
    _meritFunction=eFullFrameMaxError;
    _parameters.clear();
}
////////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::set_device(OpticalDevice* pDevice)
{
    _pDevice=pDevice;
}
//////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::add_parameter(int iSurface,string sParameter)
{
    double dMin=0.;
    double dMax=0.;

    assert(_pDevice);

    if(sParameter=="Conic")
    {
        dMin=_pDevice->get(iSurface,CONIC)-CONIC_HALF_RANGE;
        dMax=_pDevice->get(iSurface,CONIC)+CONIC_HALF_RANGE;
    }

    if(sParameter=="RCurv")
    {
        dMin=_pDevice->get(iSurface,RADIUS_CURVATURE)/RCURV_HALF_RATIO;
        dMax=_pDevice->get(iSurface,RADIUS_CURVATURE)*RCURV_HALF_RATIO;
    }

    if(sParameter=="R4")
    {
        dMin=_pDevice->get(iSurface,R4)/R4_HALF_RATIO; // todo manage R4=0
        dMax=_pDevice->get(iSurface,R4)*R4_HALF_RATIO;
    }

    if(sParameter=="R6")
    {
        dMin=_pDevice->get(iSurface,R6)/R6_HALF_RATIO;  // todo manage R6=0
        dMax=_pDevice->get(iSurface,R6)*R6_HALF_RATIO;
    }

    if(sParameter=="R8")
    {
        dMin=_pDevice->get(iSurface,R8)/R8_HALF_RATIO;  // todo manage R8=0
        dMax=_pDevice->get(iSurface,R8)*R8_HALF_RATIO;
    }

    if(sParameter=="R10")
    {
        dMin=_pDevice->get(iSurface,R10)/R10_HALF_RATIO;  // todo manage R10=0
        dMax=_pDevice->get(iSurface,R10)*R10_HALF_RATIO;
    }

    if(sParameter=="Z")
    {
        dMin=_pDevice->get(iSurface,Z)-Z_HALF_RANGE;
        dMax=_pDevice->get(iSurface,Z)*Z_HALF_RANGE;
    }

    if(sParameter=="THICK")
    {
        dMin=_pDevice->get(iSurface,THICK)-THICK_HALF_RANGE;
        dMax=_pDevice->get(iSurface,THICK)*THICK_HALF_RANGE;
    }

    add_parameter(iSurface,sParameter,dMin,dMax);
}
//////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::add_parameter(int iSurface,string sParameter,double dMin,double dMax)
{
    DeviceOptimizerParameter dop;

    if(sParameter=="RCurv") //always optimise with Curvature instead of RCurv
    {
        sParameter="Curvature";
        dMin=1./dMin;
        dMax=1./dMax;
    }

    dop.iSurface=iSurface;
    dop.sParameter=sParameter;

    if(dMin<dMax)
    {
        dop.dMin=dMin;
        dop.dMax=dMax;
    }
    else
    {
        dop.dMin=dMax;
        dop.dMax=dMin;
    }

    dop.dVal=(dMin+dMax)/2.;
    _parameters.push_back(dop);
}
//////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::set_merit_function(OptimizerMeritFunction eMeritFunction)
{
    _meritFunction=eMeritFunction;
}
//////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::apply_parameter(const ParameterSet& parameters)
{
    assert(_pDevice!=0);

    for(unsigned int i=0;i<parameters.size();i++)
    {
        string sParam=parameters[i].sParameter;
        int iSurface=parameters[i].iSurface;
        double dVal=parameters[i].dVal;

        if(sParam=="Conic")
            _pDevice->set(iSurface,CONIC,dVal);

        if(sParam=="RCurv")
            _pDevice->set(iSurface,RADIUS_CURVATURE,dVal);

        if(sParam=="Curvature")
            _pDevice->set(iSurface,CURVATURE,dVal);

        if(sParam=="Z")
            _pDevice->set(iSurface,Z,dVal);

        if(sParam=="Thick")
            _pDevice->set(iSurface,THICK,dVal);

        if(sParam=="R4")
            _pDevice->set(iSurface,R4,dVal);

        if(sParam=="R6")
            _pDevice->set(iSurface,R6,dVal);

        if(sParam=="R8")
            _pDevice->set(iSurface,R8,dVal);

        if(sParam=="R10")
            _pDevice->set(iSurface,R10,dVal);
    }
}
//////////////////////////////////////////////////////////////////////////////
double DeviceOptimizer::compute_demerit()
{
    ImageQuality pQ=_pDevice->get_image_quality();

    for(int i=0;i<pQ.nb_angles();i++)
    {
        if(isnan(pQ.vdSpotSize[i]))
            return SPOT_SIZE_INFINITY;
    }

    if(isnan(pQ.dAirySize))
        return SPOT_SIZE_INFINITY;

    if(_meritFunction==eCenterOnly)
        return pQ.vdSpotSize[0];

    if( _meritFunction==eFullFrameMean )
    {
        double dMeritMoy=0.;
        for(int i=0;i<pQ.nb_angles();i++)
            dMeritMoy+=pQ.vdSpotSize[i];

        return dMeritMoy;// /pQ.nb_angles();
    }

    if(_meritFunction==eMostlyCenter)
    {
        double dCentralWeight=3.;
        double dMeritMoy=0.;
        for(int i=0;i<pQ.nb_angles();i++)
        {
            double dFactor=1.+(pQ.nb_angles()-1.-i)/(double)(pQ.nb_angles()-1.)*(dCentralWeight-1.);
            dMeritMoy+=pQ.vdSpotSize[i]*dFactor;
        }
        return dMeritMoy; //*K
    }

    if(_meritFunction==eFullFrameMaxError)
    {
        double dMeritMoy=0.;
        for(int i=0;i<pQ.nb_angles();i++)
            if(dMeritMoy<pQ.vdSpotSize[i])
                dMeritMoy=pQ.vdSpotSize[i];

        return dMeritMoy;
    }

    return SPOT_SIZE_INFINITY;
}
//////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::compute_min_resolution()
{
    for(unsigned int i=0;i<_parameters.size();i++)
    {
        DeviceOptimizerParameter& param=_parameters[i];

        if(param.sParameter=="Conic")
            param.dResolution=MIN_RESOLUTION_CONIC;

        if(param.sParameter=="RCurv")
            param.dResolution=MIN_RESOLUTION_RCURV;

        if(param.sParameter=="Curvature")
            param.dResolution=MIN_RESOLUTION_CURVATURE;

        if(param.sParameter=="thick")
            param.dResolution=MIN_RESOLUTION_THICK;

        if(param.sParameter=="Z")
            param.dResolution=MIN_RESOLUTION_THICK;

        if(param.sParameter=="R4")
            param.dResolution=MIN_RESOLUTION_R4;

        if(param.sParameter=="R6")
            param.dResolution=MIN_RESOLUTION_R6;

        if(param.sParameter=="R8")
            param.dResolution=MIN_RESOLUTION_R8;

        if(param.sParameter=="R10")
            param.dResolution=MIN_RESOLUTION_R10;
    }
}
//////////////////////////////////////////////////////////////////////////////
bool DeviceOptimizer::domain_under_resolution(const ParameterSet& params)
{
    for(unsigned int i=0;i<params.size();i++)
    {
        const DeviceOptimizerParameter& param=params[i];
        if((param.dMax-param.dMin)>param.dResolution)
            return false;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////



