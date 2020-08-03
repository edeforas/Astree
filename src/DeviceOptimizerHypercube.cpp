// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceOptimizerHypercube.h"

#include <cassert>
////////////////////////////////////////////////////////////////////////////////
DeviceOptimizerHypercube::DeviceOptimizerHypercube():
    DeviceOptimizer()
{ }

//////////////////////////////////////////////////////////////////////////////
DeviceOptimizerHypercube::~DeviceOptimizerHypercube()
{ }
//////////////////////////////////////////////////////////////////////////////
OptimizerResult DeviceOptimizerHypercube::optimize()
{
    // divide hypercube into regular domain
    assert(_pDevice!=0);

    double dMeritOrig=compute_demerit();
    OpticalDevice deviceOrig(*_pDevice);

    if(_parameters.empty())
        return eNothingToOptimize;

    ParameterSet paramBest=_parameters;

    // compute nb of iterations
    int iNbIter=1;
    for(unsigned int i=0;i<paramBest.size();i++)
        iNbIter*=DIVIDE_ITER_BY_AXIS;

    double dBestMerit=SPOT_SIZE_INFINITY; // TODO init with actual solution

    bool bExit=false;
    while(!bExit)
    {
        ParameterSet paramBestIter=paramBest;
		vector<int> vDivide(paramBest.size(), 0);

        for(int i=0;i<iNbIter;i++)
        {
            ParameterSet paramBestTest=paramBest;
            for(unsigned int iP=0;iP<paramBestTest.size();iP++)
            {
                DeviceOptimizerParameter& dop=paramBestTest[iP];

                dop.dValue=dop.dMin+(dop.dMax-dop.dMin)*vDivide[iP]/ DIVIDE_ITER_BY_AXIS;
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
        
			//change next parameter in vDivide
			for (int i = 0; i < vDivide.size();i++)
			{
				if (vDivide[i] != DIVIDE_ITER_BY_AXIS)
				{
					vDivide[i]++;
					break;
				}
				else
					vDivide[i] = 0; // and jump next parameter
			}


		}

        paramBest=paramBestIter;

        // scale around best solution , divide by 2 each dimension test if box is too small then exit
		bExit = true;
        for(unsigned int iP=0;iP<paramBest.size();iP++)
        {
            DeviceOptimizerParameter& dop=paramBest[iP];
            double dCenter=dop.dValue;
            double dRadius=(dop.dMax-dop.dMin)/4.;

            dop.dMin=dCenter-dRadius;
            dop.dMax=dCenter+dRadius;

            // check domain exit
			if ((dop.dMax - dop.dMin) > dop.dResolution)
				bExit= false;
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
