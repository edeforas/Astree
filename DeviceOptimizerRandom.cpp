// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceOptimizerRandom.h"

#include <cassert>
////////////////////////////////////////////////////////////////////////////////
DeviceOptimizerRandom::DeviceOptimizerRandom():
	DeviceOptimizer()
{ }

//////////////////////////////////////////////////////////////////////////////
DeviceOptimizerRandom::~DeviceOptimizerRandom()
{ }
//////////////////////////////////////////////////////////////////////////////
OptimizerResult DeviceOptimizerRandom::optimize()
{
    assert(_pDevice!=0);

    if(_parameters.empty())
        return eNothingToOptimize;

    double dMeritOrig=compute_demerit();
    OpticalDevice deviceOrig(*_pDevice);

    ParameterSet paramBest=_parameters;

    // compute nb of iterations
    int iNbIter=1;
    for(unsigned int i=0;i<paramBest.size();i++)
        iNbIter*=RANDOM_ITER_BY_AXIS; //todo check overflow

    double dBestMerit=SPOT_SIZE_INFINITY; // todo init with actual solution

    bool bExit=false;
    while(!bExit)
    {
        ParameterSet paramBestIter=paramBest;
        for(int i=0;i<iNbIter;i++)
        {
            ParameterSet paramBestTest=paramBest;
            for(unsigned int iP=0;iP<paramBestTest.size();iP++)
            {
                DeviceOptimizerParameter& dop=paramBestTest[iP];

                dop.dValue=dop.dMin+(dop.dMax-dop.dMin)*rand()/RAND_MAX;
                assert(dop.dValue<=dop.dMax);
                assert(dop.dValue>=dop.dMin);
            }

            apply_parameter(paramBestTest);
            double dMerit=compute_demerit();

            if(dMerit<dBestMerit)
            {
                paramBestIter=paramBestTest;
                dBestMerit=dMerit;
            }
        }

        paramBest=paramBestIter;

        // scale around best solution , divide by 2 each dimension
        for(unsigned int iP=0;iP<paramBest.size();iP++)
        {
            DeviceOptimizerParameter& dop=paramBest[iP];
            double dCenter=dop.dValue;
            double dRadius=(dop.dMax-dop.dMin)/4.;

            dop.dMin=dCenter-dRadius;
            dop.dMax=dCenter+dRadius;

            // todo check domain exit
        }

        bExit=domain_under_resolution(paramBest);
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
