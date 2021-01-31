# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceOptimizer.h"
#include "OpticalDevice.h"

#include <cmath> #for isnan
#include <cassert>
########################################
DeviceOptimizer.DeviceOptimizer():
    _pDevice(0),
    _meritFunction(eFullFrameMaxError)
    _dMinVignetting=100.

########################################
DeviceOptimizer.~DeviceOptimizer()
{
########################################
def clear(self):
    _pDevice=0
    _meritFunction=eFullFrameMaxError
    _parameters.clear()

########################################
def set_device(self, pDevice):
    _pDevice=pDevice

#######################################
def set_min_vignetting(self, dMinVignetting):
    _dMinVignetting=dMinVignetting

#######################################
def add_parameter(self, iSurface, sParameter, bRefine):
    double dMin=0.
    double dMax=0.

    assert(_pDevice)

    if bRefine:
        if sParameter=="Conic":
            dMin=_pDevice.get(iSurface,CONIC)-CONIC_HALF_RANGE_REFINE
            dMax=_pDevice.get(iSurface,CONIC)+CONIC_HALF_RANGE_REFINE


        if sParameter=="RCurv":
            dMin=_pDevice.get(iSurface,RADIUS_CURVATURE)/RCURV_HALF_RATIO_REFINE
            dMax=_pDevice.get(iSurface,RADIUS_CURVATURE)*RCURV_HALF_RATIO_REFINE


		if sParameter == "R4":
			dR4 = _pDevice.get(iSurface, R4)
			if dR4 != 0.:
				dMin = dR4 / R4_HALF_RATIO_REFINE
				dMax = dR4 * R4_HALF_RATIO_REFINE

			else:
				dMin = -R4_HALF_RANGE_DEFAULT
				dMax = R4_HALF_RANGE_DEFAULT



        if sParameter=="R6":
			dR6 = _pDevice.get(iSurface, R6)
			if dR6 != 0.:
				dMin = dR6 / R6_HALF_RATIO_REFINE
				dMax = dR6 * R6_HALF_RATIO_REFINE

			else:
				dMin = -R6_HALF_RANGE_DEFAULT
				dMax = R6_HALF_RANGE_DEFAULT



        if sParameter=="R8":
			dR8 = _pDevice.get(iSurface, R8)
			if dR8 != 0.:
				dMin = dR8 / R8_HALF_RATIO_REFINE
				dMax = dR8 * R8_HALF_RATIO_REFINE

			else:
				dMin = -R8_HALF_RANGE_DEFAULT
				dMax = R8_HALF_RANGE_DEFAULT



        if sParameter=="R10":
			dR10 = _pDevice.get(iSurface, R10)
			if dR10 != 0.:
				dMin = dR10 / R10_HALF_RATIO_REFINE
				dMax = dR10 * R10_HALF_RATIO_REFINE

			else:
				dMin = -R10_HALF_RANGE_DEFAULT
				dMax = R10_HALF_RANGE_DEFAULT



        if sParameter=="Z":
            dMin=_pDevice.get(iSurface,Z)-Z_HALF_RANGE_REFINE
            dMax=_pDevice.get(iSurface,Z)*Z_HALF_RANGE_REFINE


        if sParameter=="Thick":
            dMin=_pDevice.get(iSurface,THICK)-THICK_HALF_RANGE_REFINE
            dMax=_pDevice.get(iSurface,THICK)+THICK_HALF_RANGE_REFINE


    else:
        if sParameter=="Conic":
            dMin=_pDevice.get(iSurface,CONIC)-CONIC_HALF_RANGE
            dMax=_pDevice.get(iSurface,CONIC)+CONIC_HALF_RANGE


        if sParameter=="RCurv":
            dMin=_pDevice.get(iSurface,RADIUS_CURVATURE)/RCURV_HALF_RATIO
            dMax=_pDevice.get(iSurface,RADIUS_CURVATURE)*RCURV_HALF_RATIO


		if sParameter == "R4":
			dR4 = _pDevice.get(iSurface, R4)
			if dR4 != 0.:
				dMin = dR4 / R4_HALF_RATIO
				dMax = dR4 * R4_HALF_RATIO

			else:
				dMin = -R4_HALF_RANGE_DEFAULT
				dMax = R4_HALF_RANGE_DEFAULT



		if sParameter == "R6":
			dR6 = _pDevice.get(iSurface, R6)
			if dR6 != 0.:
				dMin = dR6 / R6_HALF_RATIO
				dMax = dR6 * R6_HALF_RATIO

			else:
				dMin = -R6_HALF_RANGE_DEFAULT
				dMax = R6_HALF_RANGE_DEFAULT



		if sParameter == "R8":
			dR8 = _pDevice.get(iSurface, R8)
			if dR8 != 0.:
				dMin = dR8 / R8_HALF_RATIO
				dMax = dR8 * R8_HALF_RATIO

			else:
				dMin = -R8_HALF_RANGE_DEFAULT
				dMax = R8_HALF_RANGE_DEFAULT



		if sParameter == "R10":
			dR10 = _pDevice.get(iSurface, R10)
			if dR10 != 0.:
				dMin = dR10 / R10_HALF_RATIO
				dMax = dR10 * R10_HALF_RATIO

			else:
				dMin = -R10_HALF_RANGE_DEFAULT
				dMax = R10_HALF_RANGE_DEFAULT



        if sParameter=="Z":
            dMin=_pDevice.get(iSurface,Z)-Z_HALF_RANGE
            dMax=_pDevice.get(iSurface,Z)*Z_HALF_RANGE


        if sParameter=="Thick":
            dMin=_pDevice.get(iSurface,THICK)-THICK_HALF_RANGE
            dMax=_pDevice.get(iSurface,THICK)+THICK_HALF_RANGE



    add_parameter(iSurface,sParameter,dMin,dMax)

#######################################
def add_parameter(self, iSurface, sParameter, dMin, dMax):
    DeviceOptimizerParameter dop

    if(sParameter=="RCurv") #always optimise with Curvature instead of RCurv
		dop.sParameter ="Curvature"
        dMin=1./dMin
        dMax=1./dMax

	else:
		dop.sParameter = sParameter

    dop.iSurface=iSurface

    if dMin<dMax:
        dop.dMin=dMin
        dop.dMax=dMax

    else:
        dop.dMin=dMax
        dop.dMax=dMin


    dop.dValue=(dMin+dMax)/2.

    if sParameter=="Conic":
        dop.dResolution=MIN_RESOLUTION_CONIC

    if sParameter=="RCurv":
        dop.dResolution=MIN_RESOLUTION_RCURV

    if sParameter=="Curvature":
        dop.dResolution=MIN_RESOLUTION_CURVATURE

    if sParameter=="thick":
        dop.dResolution=MIN_RESOLUTION_THICK

    if sParameter=="Z":
        dop.dResolution=MIN_RESOLUTION_THICK

    if sParameter=="R4":
        dop.dResolution=MIN_RESOLUTION_R4

    if sParameter=="R6":
        dop.dResolution=MIN_RESOLUTION_R6

    if sParameter=="R8":
        dop.dResolution=MIN_RESOLUTION_R8

    if sParameter=="R10":
        dop.dResolution=MIN_RESOLUTION_R10

    _parameters.push_back(dop)

#######################################
def set_merit_function(self, eMeritFunction):
    _meritFunction=eMeritFunction

#######################################
def apply_parameter(self, parameters):
    assert(_pDevice!=0)

    for(unsigned int i=0;i<parameters.size();i++)
        string sParam=parameters[i].sParameter
        int iSurface=parameters[i].iSurface
        double dVal=parameters[i].dValue

        if sParam=="Conic":
            _pDevice.set(iSurface,CONIC,dVal)

        if sParam=="RCurv":
            _pDevice.set(iSurface,RADIUS_CURVATURE,dVal)

        if sParam=="Curvature":
            _pDevice.set(iSurface,CURVATURE,dVal)

        if sParam=="Z":
            _pDevice.set(iSurface,Z,dVal)

        if sParam=="Thick":
            _pDevice.set(iSurface,THICK,dVal)

        if sParam=="R4":
            _pDevice.set(iSurface,R4,dVal)

        if sParam=="R6":
            _pDevice.set(iSurface,R6,dVal)

        if sParam=="R8":
            _pDevice.set(iSurface,R8,dVal)

        if sParam=="R10":
            _pDevice.set(iSurface,R10,dVal)


#######################################
def compute_demerit(self):
    ImageQuality pQ=_pDevice.get_image_quality()

    for(int i=0;i<pQ.nb_angles();i++)
        if std.isnan(pQ.vdSpotSize[i]):
            return SPOT_SIZE_INFINITY

        if std.isnan(pQ.vdSpotvsAiry[i]):
            return SPOT_SIZE_INFINITY

        if pQ.vdVignetting[i]<_dMinVignetting:
            return SPOT_SIZE_INFINITY


    if std.isnan(pQ.dAirySize):
        return SPOT_SIZE_INFINITY

    if _meritFunction==eCenterOnly:
        return pQ.vdSpotSize[0]

    if  _meritFunction==eFullFrameMean :
        double dMeritMoy=0.
        for(int i=0;i<pQ.nb_angles();i++)
            dMeritMoy+=pQ.vdSpotSize[i]

        return dMeritMoy


    if _meritFunction==eMostlyCenter:
        double dCentralWeight=3.
        double dMeritMoy=0.
        for(int i=0;i<pQ.nb_angles();i++)
            double dFactor=1.+(pQ.nb_angles()-1.-i)/(double)(pQ.nb_angles()-1.)*(dCentralWeight-1.)
            dMeritMoy+=pQ.vdSpotSize[i]*dFactor

        return dMeritMoy; #*K


    if _meritFunction==eFullFrameMaxError:
        double dMeritMoy=0.
        for(int i=0;i<pQ.nb_angles();i++)
            if dMeritMoy<pQ.vdSpotSize[i]:
                dMeritMoy=pQ.vdSpotSize[i]

        return dMeritMoy


    return SPOT_SIZE_INFINITY

#######################################
