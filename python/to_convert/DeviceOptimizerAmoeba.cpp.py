# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceOptimizerAmoeba.h"

#include <cmath>
#include <cassert>
########################################
DeviceOptimizerAmoeba.DeviceOptimizerAmoeba():
    DeviceOptimizer()
{
########################################
DeviceOptimizerAmoeba.~DeviceOptimizerAmoeba()
{
#######################################
def optimize(self):
    # see algorithm at : https:#en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method
    assert(_pDevice!=0)

    double dMeritOrig=compute_demerit()
    OpticalDevice deviceOrig(*_pDevice)

    if _parameters.empty():
        return eNothingToOptimize

    # init simplex with the center of the definition domain
    vector<ParameterSet> simplex(_parameters.size()+1)
        ParameterSet& param=simplex[0]
        param=_parameters
        for(unsigned int i=0;i<_parameters.size();i++)
            param[i].dValue=(param[i].dMin+param[i].dMax)*0.5



    # then init simplex with the semi edge of the definition domain
    for(unsigned int i=1;i<simplex.size();i++)
        ParameterSet& param=simplex[i]
        param=simplex[0]

        # expand the i-1 axis
        DeviceOptimizerParameter& paramToExpand=param[i-1]
        paramToExpand.dValue=paramToExpand.dValue+(paramToExpand.dMax-paramToExpand.dMin)/8.


    #compute solution at each param
    vector<double> vdDemerit(simplex.size())
    for(unsigned int i=0;i<simplex.size();i++)
        apply_parameter(simplex[i])
        vdDemerit[i]=compute_demerit()


    int iIter=0,iMaxIter=AMOEBA_MAX_ITER
    bool bStopCriteria=False
    unsigned int iBest=0
    double dBestMerit=vdDemerit[0]
    while((iIter<iMaxIter) and (bStopCriteria==False))
        #get the best, and secondworse solution index
        dBestMerit=vdDemerit[0]
        iBest=0
        int iWorse=0
        double dWorse=vdDemerit[0]
        int iSecondWorse=0; (void)iSecondWorse
        double dSecondWorse=vdDemerit[0]
        for(unsigned int i=1;i<simplex.size();i++)
            if vdDemerit[i]<dBestMerit:
                iBest=i
                dBestMerit=vdDemerit[i]


            if vdDemerit[i]>dWorse:
                iWorse=i
                dWorse=vdDemerit[i]


        #find the second worse
        for(unsigned int i=1;i<simplex.size();i++)
            if  ((int)i!=iWorse) and (vdDemerit[i]>dSecondWorse) :
                #   iSecondWorse=i; #TODO not used?
                dSecondWorse=vdDemerit[i]


        assert(dBestMerit<=dSecondWorse)
        assert(dSecondWorse<=dWorse)

        #search the best solution on the line (worse,mean)
         ParameterSet& paramWorse=simplex[iWorse]

        #compute param mean
        ParameterSet paramMean=paramWorse
        for(unsigned int i=0;i<paramMean.size();i++)
            paramMean[i].dValue=0.
        for(unsigned int iS=0;iS<simplex.size();iS++)
            if (int)iS==iWorse:
                continue

             ParameterSet& param=simplex[iS]
            for(unsigned int i=0;i<param.size();i++)
                paramMean[i].dValue+=param[i].dValue

        for(unsigned int i=0;i<paramMean.size();i++)
            paramMean[i].dValue/=simplex.size()-1.

        bool bFound=False

        #reflexion case
        ParameterSet paramMirror=paramMean
        bool bOutOfDomain=False
        for(unsigned int i=0;i<paramMirror.size();i++)
            paramMirror[i].dValue+=(paramMean[i].dValue-paramWorse[i].dValue)
            if  (paramMirror[i].dValue<paramMirror[i].dMin ) or (paramMirror[i].dValue>paramMirror[i].dMax ) :
                bOutOfDomain=True


        double dMirror=SPOT_SIZE_INFINITY
        if not bOutOfDomain:
            apply_parameter(paramMirror)
            dMirror=compute_demerit()


        if  (dBestMerit<=dMirror) and (dMirror<dSecondWorse) :
            #replace Worst by mirror
            simplex[iWorse]=paramMirror
            vdDemerit[iWorse]=dMirror
            bFound=True


        #expansion
        if not bFound and (dMirror<dBestMerit):
            #compute paramMirrorFar
            ParameterSet paramMirrorFar=paramMean
            bool bOutOfDomain2=False
            for(unsigned int i=0;i<paramMirrorFar.size();i++)
                paramMirrorFar[i].dValue+=2*(paramMirror[i].dValue-paramMean[i].dValue)
                if  (paramMirrorFar[i].dValue<paramMirrorFar[i].dMin ) or (paramMirrorFar[i].dValue>paramMirrorFar[i].dMax ) :
                    bOutOfDomain2=True

            double dMirrorFar=SPOT_SIZE_INFINITY
            if not bOutOfDomain2:
                apply_parameter(paramMirrorFar)
                dMirrorFar=compute_demerit()


            if dMirrorFar<dBestMerit:
                simplex[iWorse]=paramMirrorFar
                vdDemerit[iWorse]=dMirrorFar
                bFound=True

            else:
                simplex[iWorse]=paramMirror
                vdDemerit[iWorse]=dMirror
                bFound=True



        #contraction
        if not bFound:
            assert(dMirror>=dSecondWorse)
            ParameterSet paramMirrorContracted=paramMean
            for(unsigned int i=0;i<paramMirrorContracted.size();i++)
                paramMirrorContracted[i].dValue+=0.5*(paramWorse[i].dValue-paramMean[i].dValue)
            apply_parameter(paramMirrorContracted)
            double dContracted=compute_demerit()
            if dContracted<dWorse:
                simplex[iWorse]=paramMirrorContracted
                vdDemerit[iWorse]=dContracted
                bFound=True



        #reduction
        if not bFound:
            for(unsigned int i=0;i<simplex.size();i++)
                 ParameterSet& paramBest=simplex[iBest]
                if i!=iBest:
                    ParameterSet& paramReducted=simplex[i]
                    for(unsigned int j=0;j<paramReducted.size();j++)
                        paramReducted[j].dValue=paramBest[j].dValue+0.5*(paramReducted[j].dValue-paramBest[j].dValue)

                    apply_parameter(paramReducted)
                    vdDemerit[i]=compute_demerit()


            bFound=True; #  keep for debug


        assert(bFound)

        #compute the parameter range
        # stop criteria based on the simplex size
        vector<double> vdMinParam(_parameters.size(),SPOT_SIZE_INFINITY)
        vector<double> vdMaxParam(_parameters.size(),-SPOT_SIZE_INFINITY)
        for(unsigned int i=0;i<simplex.size();i++)
             ParameterSet& param=simplex[i]
            for(unsigned int j=0;j<param.size();j++)
                if param[j].dValue<vdMinParam[j]:
                    vdMinParam[j]=param[j].dValue
                if param[j].dValue>vdMaxParam[j]:
                    vdMaxParam[j]=param[j].dValue



        bStopCriteria=True
        for(unsigned int i=0;i<vdMinParam.size();i++)
            if vdMaxParam[i]-vdMinParam[i]>_parameters[i].dResolution:
                bStopCriteria=False


        iIter++


    if dBestMerit<SPOT_SIZE_INFINITY/2:
        if  (dBestMerit<dMeritOrig) and (iIter>=0) :
            _parameters=simplex[iBest]
            apply_parameter(_parameters)
            return eBetterSolutionFound


    #restore device original settings
    *_pDevice=deviceOrig
    return eNoBetterSolution

#######################################
