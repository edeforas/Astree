// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceOptimizer.h"
#include "OpticalDevice.h"

#define ISNAN(a) (a!=a)

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

        if(sParam=="Conic")
            _pDevice->set(iSurface,CONIC,dVal);

        if(sParam=="RCurv")
            _pDevice->set(iSurface,RADIUS_CURVATURE,dVal);

        if(sParam=="Thick")
            _pDevice->set_z(iSurface,dVal);

        if(sParam=="r4")
            _pDevice->set(iSurface,R4,dVal);

        if(sParam=="r6")
            _pDevice->set(iSurface,R6,dVal);

        if(sParam=="r8")
            _pDevice->set(iSurface,R8,dVal);

        if(sParam=="r10")
            _pDevice->set(iSurface,R10,dVal);

    }
}
//////////////////////////////////////////////////////////////////////////////
double DeviceOptimizer::compute_demerit(OptimizerMeritFunction eMeritFunction)
{
    const ImageQuality* pQ=_pDevice->get_image_quality();

    for(int i=0;i<pQ->nb_angles();i++)
    {
        if(ISNAN(pQ->vdSpotSize[i]))
            return SPOT_SIZE_INFINITY;
    }

    if(ISNAN(pQ))
        return SPOT_SIZE_INFINITY;

    if(eMeritFunction==eCenterOnly)
        return pQ->vdSpotSize[0];

    if(eMeritFunction==eFullFrameMean)
    {
        double dMeritMoy=0.;
        for(int i=0;i<pQ->nb_angles();i++)
            dMeritMoy+=pQ->vdSpotSize[i];

        return dMeritMoy/pQ->nb_angles();
    }

    if(eMeritFunction==eMostlyCenter)
    {
        return SPOT_SIZE_INFINITY; //TODO
    }

    if(eMeritFunction==eFullFrameMaxError)
    {
        double dMeritMoy=0.;
        for(int i=0;i<pQ->nb_angles();i++)
            if(dMeritMoy<pQ->vdSpotSize[i])
                dMeritMoy=pQ->vdSpotSize[i];

        return dMeritMoy;
    }

    return SPOT_SIZE_INFINITY;
}
//////////////////////////////////////////////////////////////////////////////
OptimizerResult DeviceOptimizer::optimise_random(OptimizerMeritFunction eMeritFunction)
{
    assert(_pDevice!=0);

    if(_parameters.empty())
        return eNothingToOptimize;

    double dMeritOrig=compute_demerit(eMeritFunction);
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
            double dMerit=compute_demerit(eMeritFunction);

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
OptimizerResult DeviceOptimizer::optimise_amoeba(OptimizerMeritFunction eMeritFunction)
{
    // see algo at : https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method

    assert(_pDevice!=0);

    double dMeritOrig=compute_demerit(eMeritFunction);
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

        if(_parameters[i].sParameter=="thick")
            vdMinResolution[i]=MIN_RESOLUTION_THICK;

        if(_parameters[i].sParameter=="r4")
            vdMinResolution[i]=MIN_RESOLUTION_R4;

        if(_parameters[i].sParameter=="r6")
            vdMinResolution[i]=MIN_RESOLUTION_R6;

        if(_parameters[i].sParameter=="r8")
            vdMinResolution[i]=MIN_RESOLUTION_R8;

        if(_parameters[i].sParameter=="r10")
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
        vdDemerit[i]=compute_demerit(eMeritFunction);
    }

    int iIter=0,iMaxIter=AMOEBA_MAX_ITER;
    bool bStopCriteria=false;
    int iBest=0;
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
                iSecondWorse=i;
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
            dMirror=compute_demerit(eMeritFunction);
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
                dMirrorFar=compute_demerit(eMeritFunction);
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
            double dContracted=compute_demerit(eMeritFunction);
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
                if((int)i!=iBest)
                {
                    ParameterSet& paramReducted=simplex[i];
                    for(unsigned int j=0;j<paramReducted.size();j++)
                        paramReducted[j].dVal=paramBest[j].dVal+0.5*(paramReducted[j].dVal-paramBest[j].dVal);

                    apply_parameter(paramReducted);
                    vdDemerit[i]=compute_demerit(eMeritFunction);
                }
            }
            bFound=true;
        }

        assert(bFound==true);

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
