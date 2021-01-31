# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceOptimizerRandom.h"

#include <cassert>
########################################
DeviceOptimizerRandom.DeviceOptimizerRandom():
    DeviceOptimizer()
{

#######################################
DeviceOptimizerRandom.~DeviceOptimizerRandom()
{
#######################################
def optimize(self):
    # random choice in hypercube (nb test:RANDOM_ITER_BY_AXIS^^N) then reduce on best
    assert(_pDevice!=0)

    double dMeritOrig=compute_demerit()
    OpticalDevice deviceOrig(*_pDevice)

    if _parameters.empty():
        return eNothingToOptimize

    ParameterSet paramBest=_parameters

    # compute nb of iterations
    int iNbIter=1
    for(unsigned int i=0;i<paramBest.size();i++)
        iNbIter*=RANDOM_ITER_BY_AXIS; #TODO check overflow

    double dBestMerit=SPOT_SIZE_INFINITY; # TODO init with actual solution

    bool bExit=False
    while(not bExit)
        ParameterSet paramBestIter=paramBest
        for(int i=0;i<iNbIter;i++)
            ParameterSet paramBestTest=paramBest
            for(unsigned int iP=0;iP<paramBestTest.size();iP++)
                DeviceOptimizerParameter& dop=paramBestTest[iP]

                dop.dValue=dop.dMin+(dop.dMax-dop.dMin)*rand()/RAND_MAX
                assert(dop.dValue<=dop.dMax)
                assert(dop.dValue>=dop.dMin)


            apply_parameter(paramBestTest)
            double dMerit=compute_demerit()

            if dMerit<dBestMerit:
                paramBestIter=paramBestTest
                dBestMerit=dMerit



        paramBest=paramBestIter

        # scale around best solution , by 2 each dimension test if box is too small then exit
		bExit = True
        for(unsigned int iP=0;iP<paramBest.size();iP++)
            DeviceOptimizerParameter& dop=paramBest[iP]
            double dCenter=dop.dValue
            double dRadius=(dop.dMax-dop.dMin)/4.

            dop.dMin=dCenter-dRadius
            dop.dMax=dCenter+dRadius

            # check domain exit
			if (dop.dMax - dop.dMin) > dop.dResolution:
				bExit= False



    if dBestMerit<SPOT_SIZE_INFINITY/2:
        if dBestMerit<dMeritOrig:
            _parameters=paramBest
            apply_parameter(paramBest)
            return eBetterSolutionFound



    #restore device original settings
    *_pDevice=deviceOrig
    return eNoBetterSolution

#######################################
