// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceOptimizerAmoeba.h"

#include <cmath> //for isnan
#include <cassert>
////////////////////////////////////////////////////////////////////////////////
DeviceOptimizerAmoeba::DeviceOptimizerAmoeba():
	DeviceOptimizer()
{ }
////////////////////////////////////////////////////////////////////////////////
DeviceOptimizerAmoeba::~DeviceOptimizerAmoeba()
{ }
//////////////////////////////////////////////////////////////////////////////
OptimizerResult DeviceOptimizerAmoeba::optimize()
{
    // see algo at : https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method

    assert(_pDevice!=0);

    double dMeritOrig=compute_demerit();
    OpticalDevice deviceOrig(*_pDevice);

    if(_parameters.empty())
        return eNothingToOptimize;

    //initialise parameters min resolution
    compute_min_resolution();

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
            if(vdMaxParam[i]-vdMinParam[i]>_vdMinResolution[i])
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
