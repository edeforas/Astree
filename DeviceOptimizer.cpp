// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceOptimizer.h"
#include "OpticalDevice.h"

#include <cassert>
////////////////////////////////////////////////////////////////////////////////
DeviceOptimizer::DeviceOptimizer()
{
    _pDevice=0;
}
////////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::set_device(OpticalDevice* pDevice)
{
    _pDevice=pDevice;
}
//////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::add_parameter(int iSurface,string sParameter,double dMin,double dMax)
{
    DeviceOptimizerParameter dop;
    dop.iSurface=iSurface;
    dop.sParameter=sParameter;
    dop.dMin=dMin;
    dop.dMax=dMax;
    dop.dVal=(dMin+dMax)/2.;
    _parameters.push_back(dop);
}
//////////////////////////////////////////////////////////////////////////////
void DeviceOptimizer::apply_parameter(const vector<DeviceOptimizerParameter>& parameters)
{
    assert(_pDevice!=0);

    for(unsigned int i=0;i<parameters.size();i++)
    {
        string sParam=parameters[i].sParameter;
        int iSurface=parameters[i].iSurface;
        double dVal=parameters[i].dVal;

        if(sParam=="conic")
            _pDevice->set_conic(iSurface,dVal);

        if(sParam=="RCurv")
            _pDevice->set_radius_curvature(iSurface,dVal);

        if(sParam=="thick")
            _pDevice->set_z(iSurface,dVal);

        //TODO more params
    }
}
//////////////////////////////////////////////////////////////////////////////
double DeviceOptimizer::compute_demerit(OptimizerMeritFunction eMeritFunction)
{
    const ImageQuality* pQ=_pDevice->get_image_quality();

    if(eMeritFunction==eCenterOnly)
        return pQ->vdSpotSize[0];

    if(eMeritFunction==eFullFrameMean)
    {
        double dMeritMoy=0.;
        for(int i=0;i<pQ->nb_angles();i++)
            dMeritMoy+=pQ->vdSpotSize[i]; // todo adjust with photon influence surface

        return dMeritMoy/pQ->nb_angles();
    }

    if(eMeritFunction==eMostlyCenter)
    {
        return 1e99; //TODO
    }

    if(eMeritFunction==eFullFrameMaxError)
    {
        double dMeritMoy=0.;
        for(int i=0;i<pQ->nb_angles();i++)
            if(dMeritMoy<pQ->vdSpotSize[i])
                dMeritMoy=pQ->vdSpotSize[i];

        return dMeritMoy;
    }

    return 1e99;
}
//////////////////////////////////////////////////////////////////////////////
OptimizerResult DeviceOptimizer::optimise_random(OptimizerMeritFunction eMeritFunction)
{
    assert(_pDevice!=0);

    if(_parameters.empty())
        return eNothingToOptimize;

    double dMeritOrig=compute_demerit(eMeritFunction);
    OpticalDevice deviceOrig(*_pDevice);

    vector<DeviceOptimizerParameter> paramBest=_parameters;
    double dBestMerit=1.e99; // todo init with actual solution

    for(int iScale=0;iScale<10;iScale++)
    {
        for(int i=0;i<100;i++)
        {
            vector<DeviceOptimizerParameter> newParamBest=paramBest;
            for(unsigned int iP=0;iP<newParamBest.size();iP++)
            {
                DeviceOptimizerParameter& dop=newParamBest[iP];

                dop.dVal=dop.dMin+(dop.dMax-dop.dMin)*rand()/RAND_MAX;
                assert(dop.dVal<=dop.dMax);
                assert(dop.dVal>=dop.dMin);
            }

            apply_parameter(newParamBest);
            double dMerit=compute_demerit(eMeritFunction);

            if(dMerit<dBestMerit)
            {
                paramBest=newParamBest;
                dBestMerit=dMerit;
            }
        }

        // todo check dBestMerit

        // scale around best solution , divide by 4 each dimension
        for(unsigned int iP=0;iP<paramBest.size();iP++)
        {
            DeviceOptimizerParameter& dop=paramBest[iP];
            double dCenter=dop.dVal;
            double dRadius=(dop.dMax-dop.dMin)/2./4.;

            dop.dMin=dCenter-dRadius;
            dop.dMax=dCenter+dRadius;

            // todo check domain exit
        }







    }


    if(dBestMerit<1.e98)
    {
        if(dBestMerit<dMeritOrig)
        {
            _parameters=paramBest;
            apply_parameter(paramBest);
            return eBetterSolutionFound;
        }
    }

    // todo add amoeba optimization

    //restore device original settings
    *_pDevice=deviceOrig;
    return eNoBetterSolution;
}
//////////////////////////////////////////////////////////////////////////////
