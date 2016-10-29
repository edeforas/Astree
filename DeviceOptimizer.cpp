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

    if( (_meritFunction==eFullFrameMean) || (_meritFunction==eMostlyCenter) ) //todo remove eMostlyCenter
    {
        double dMeritMoy=0.;
        for(int i=0;i<pQ.nb_angles();i++)
            dMeritMoy+=pQ.vdSpotSize[i];

        return dMeritMoy/pQ.nb_angles();
    }
    /*
    if(_meritFunction==eMostlyCenter)
    {
        //todo find the optimal formula
        double dCentralWeight=10;
        double dMeritMoy=0.;
        for(int i=0;i<pQ.nb_angles();i++)
        {
            double dFactor=dCentralWeight+(1.-dCentralWeight)/(pQ.nb_angles()-1)*i;

            if(dMeritMoy<pQ.vdSpotSize[i])
                dMeritMoy=pQ.vdSpotSize[i]*dFactor;
        }
        return dMeritMoy;
    }
*/
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
OptimizerResult DeviceOptimizer::optimise_random()
{
    assert(_pDevice!=0);

    if(_parameters.empty())
        return eNothingToOptimize;

    double dMeritOrig=compute_demerit();
    OpticalDevice deviceOrig(*_pDevice);

    ParameterSet paramBest=_parameters;
    double dBestMerit=SPOT_SIZE_INFINITY; // todo init with actual solution

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
            double dMerit=compute_demerit();

            if(dMerit<dBestMerit)
            {
                paramBest=newParamBest;
                dBestMerit=dMerit;
            }
        }

        // scale around best solution , divide by 4 each dimension
        for(unsigned int iP=0;iP<paramBest.size();iP++)
        {
            DeviceOptimizerParameter& dop=paramBest[iP];
            double dCenter=dop.dVal;
            double dRadius=(dop.dMax-dop.dMin)/8.;

            dop.dMin=dCenter-dRadius;
            dop.dMax=dCenter+dRadius;

            // todo check domain exit
        }
    }

    if(dBestMerit<SPOT_SIZE_INFINITY/2)
    {
        if(dBestMerit<dMeritOrig)
        {
            _parameters=paramBest;
            apply_parameter(paramBest);
            return eBetterSolutionFound;
        }
    }

    //restore device original settings
    *_pDevice=deviceOrig;
    return eNoBetterSolution;
}
//////////////////////////////////////////////////////////////////////////////
OptimizerResult DeviceOptimizer::optimise_amoeba()
{
    // see algo at : https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method

    assert(_pDevice!=0);

    double dMeritOrig=compute_demerit();
    OpticalDevice deviceOrig(*_pDevice);

    if(_parameters.empty())
        return eNothingToOptimize;

    //initialise parameteres min resolution
    vector<double> vdMinResolution(_parameters.size());
    for(unsigned int i=0;i<_parameters.size();i++)
    {
        if(_parameters[i].sParameter=="conic")
            vdMinResolution[i]=MIN_RESOLUTION_CONIC;

        if(_parameters[i].sParameter=="RCurv")
            vdMinResolution[i]=MIN_RESOLUTION_RCURV;

        if(_parameters[i].sParameter=="Curvature")
            vdMinResolution[i]=MIN_RESOLUTION_CURVATURE;

        if(_parameters[i].sParameter=="thick")
            vdMinResolution[i]=MIN_RESOLUTION_THICK;

        if(_parameters[i].sParameter=="Z")
            vdMinResolution[i]=MIN_RESOLUTION_THICK;

        if(_parameters[i].sParameter=="R4")
            vdMinResolution[i]=MIN_RESOLUTION_R4;

        if(_parameters[i].sParameter=="R6")
            vdMinResolution[i]=MIN_RESOLUTION_R6;

        if(_parameters[i].sParameter=="R8")
            vdMinResolution[i]=MIN_RESOLUTION_R8;

        if(_parameters[i].sParameter=="R10")
            vdMinResolution[i]=MIN_RESOLUTION_R10;
    }

    // init simplex with the center of the definition domain
    vector<ParameterSet> simplex(_parameters.size()+1);
    {
        ParameterSet& param=simplex[0];
        param=_parameters;
        for(unsigned int i=0;i<_parameters.size();i++)
        {
            param[i].dVal=(param[i].dMin+param[i].dMax)*0.5;
        }
    }

    // then init simplex with the semi edge of the definition domain
    for(unsigned int i=1;i<simplex.size();i++)
    {
        ParameterSet& param=simplex[i];
        param=simplex[0];

        // expand the i-1 axis
        DeviceOptimizerParameter& paramToExpand=param[i-1];
        paramToExpand.dVal=paramToExpand.dVal+(paramToExpand.dMax-paramToExpand.dMin)/8.;
    }

    //compute solution at each param
    vector<double> vdDemerit(simplex.size());
    for(unsigned int i=0;i<simplex.size();i++)
    {
        apply_parameter(simplex[i]);
        vdDemerit[i]=compute_demerit();
    }

    int iIter=0,iMaxIter=AMOEBA_MAX_ITER;
    bool bStopCriteria=false;
    unsigned int iBest=0;
    double dBest=vdDemerit[0];
    while((iIter<iMaxIter) && (bStopCriteria==false))
    {
        //get the best, worse and secondworse solution index
        dBest=vdDemerit[0];
        iBest=0;
        int iWorse=0;
        double dWorse=vdDemerit[0];
        int iSecondWorse=0; (void)iSecondWorse;
        double dSecondWorse=vdDemerit[0];
        for(unsigned int i=1;i<simplex.size();i++)
        {
            if(vdDemerit[i]<dBest)
            {
                iBest=i;
                dBest=vdDemerit[i];
            }

            if(vdDemerit[i]>dWorse)
            {
                iWorse=i;
                dWorse=vdDemerit[i];
            }
        }
        //find the second worse
        for(unsigned int i=1;i<simplex.size();i++)
            if( ((int)i!=iWorse) && (vdDemerit[i]>dSecondWorse) )
            {
                //   iSecondWorse=i; //TODO not used?
                dSecondWorse=vdDemerit[i];
            }

        assert(dBest<=dSecondWorse);
        assert(dSecondWorse<=dWorse);

        //search the best solution on the line (worse,mean)
        const ParameterSet& paramWorse=simplex[iWorse];

        //compute param mean
        ParameterSet paramMean=paramWorse;
        for(unsigned int i=0;i<paramMean.size();i++)
            paramMean[i].dVal=0.;
        for(unsigned int iS=0;iS<simplex.size();iS++)
        {
            if((int)iS==iWorse)
                continue;

            const ParameterSet& param=simplex[iS];
            for(unsigned int i=0;i<param.size();i++)
                paramMean[i].dVal+=param[i].dVal;
        }
        for(unsigned int i=0;i<paramMean.size();i++)
            paramMean[i].dVal/=simplex.size()-1.;

        bool bFound=false;

        //reflexion case
        ParameterSet paramMirror=paramMean;
        bool bOutOfDomain=false;
        for(unsigned int i=0;i<paramMirror.size();i++)
        {
            paramMirror[i].dVal+=(paramMean[i].dVal-paramWorse[i].dVal);
            if( (paramMirror[i].dVal<paramMirror[i].dMin ) || (paramMirror[i].dVal>paramMirror[i].dMax ) )
                bOutOfDomain=true;
        }

        double dMirror=SPOT_SIZE_INFINITY;
        if(!bOutOfDomain)
        {
            apply_parameter(paramMirror);
            dMirror=compute_demerit();
        }

        if( (dBest<=dMirror) && (dMirror<dSecondWorse) )
        {
            //replace Worst by mirror
            simplex[iWorse]=paramMirror;
            vdDemerit[iWorse]=dMirror;
            bFound=true;
        }

        //expansion
        if(!bFound && (dMirror<dBest))
        {
            //compute paramMirrorFar
            ParameterSet paramMirrorFar=paramMean;
            bool bOutOfDomain=false;
            for(unsigned int i=0;i<paramMirrorFar.size();i++)
            {
                paramMirrorFar[i].dVal+=2*(paramMirror[i].dVal-paramMean[i].dVal);
                if( (paramMirrorFar[i].dVal<paramMirrorFar[i].dMin ) || (paramMirrorFar[i].dVal>paramMirrorFar[i].dMax ) )
                    bOutOfDomain=true;
            }
            double dMirrorFar=SPOT_SIZE_INFINITY;
            if(!bOutOfDomain)
            {
                apply_parameter(paramMirrorFar);
                dMirrorFar=compute_demerit();
            }

            if(dMirrorFar<dBest)
            {
                simplex[iWorse]=paramMirrorFar;
                vdDemerit[iWorse]=dMirrorFar;
                bFound=true;
            }
            else
            {
                simplex[iWorse]=paramMirror;
                vdDemerit[iWorse]=dMirror;
                bFound=true;
            }
        }

        //contraction
        if(!bFound)
        {
            assert(dMirror>=dSecondWorse);
            ParameterSet paramMirrorContracted=paramMean;
            for(unsigned int i=0;i<paramMirrorContracted.size();i++)
                paramMirrorContracted[i].dVal+=0.5*(paramWorse[i].dVal-paramMean[i].dVal);
            apply_parameter(paramMirrorContracted);
            double dContracted=compute_demerit();
            if(dContracted<dWorse)
            {
                simplex[iWorse]=paramMirrorContracted;
                vdDemerit[iWorse]=dContracted;
                bFound=true;
            }
        }

        //reduction
        if(!bFound)
        {
            for(unsigned int i=0;i<simplex.size();i++)
            {
                const ParameterSet& paramBest=simplex[iBest];
                if(i!=iBest)
                {
                    ParameterSet& paramReducted=simplex[i];
                    for(unsigned int j=0;j<paramReducted.size();j++)
                        paramReducted[j].dVal=paramBest[j].dVal+0.5*(paramReducted[j].dVal-paramBest[j].dVal);

                    apply_parameter(paramReducted);
                    vdDemerit[i]=compute_demerit();
                }
            }
            bFound=true; //  keep for debug
        }

        assert(bFound);

        //compute the parameter range
        // stop criteria based on the simplex size
        vector<double> vdMinParam(_parameters.size(),SPOT_SIZE_INFINITY);
        vector<double> vdMaxParam(_parameters.size(),-SPOT_SIZE_INFINITY);
        for(unsigned int i=0;i<simplex.size();i++)
        {
            const ParameterSet& param=simplex[i];
            for(unsigned int j=0;j<param.size();j++)
            {
                if(param[j].dVal<vdMinParam[j])
                    vdMinParam[j]=param[j].dVal;
                if(param[j].dVal>vdMaxParam[j])
                    vdMaxParam[j]=param[j].dVal;
            }
        }

        bStopCriteria=true;
        for(unsigned int i=0;i<vdMinParam.size();i++)
        {
            if(vdMaxParam[i]-vdMinParam[i]>vdMinResolution[i])
                bStopCriteria=false;
        }

        iIter++;
    }

    if( (dBest<dMeritOrig) && (iIter>=0) )
    {
        _parameters=simplex[iBest];
        apply_parameter(_parameters);
        return eBetterSolutionFound;
    }

    //restore device original settings
    *_pDevice=deviceOrig;
    return eNoBetterSolution;
}
//////////////////////////////////////////////////////////////////////////////
OptimizerResult DeviceOptimizer::optimise()
{
    //todo add optimise_random as an option

    return optimise_amoeba();
}
//////////////////////////////////////////////////////////////////////////////
