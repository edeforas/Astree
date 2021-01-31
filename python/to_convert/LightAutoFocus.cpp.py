# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "LightAutofocus.h"
#include "Light.h"
#include "Photon.h"

#include <algorithm>

#define SPOT_SIZE_INFINITY 1.e99

########################################
LightAutofocus.LightAutofocus()
    _xCenter=0.
    _yCenter=0.

########################################
def autofocus(self, l):
    double dB=0.
    double dA=dB-1000.
    double dC=dB+1000.
    double dZStep=1.e-9

    # find a point that is valid for initial start
    int iScale=0
    double dQA, dQC, dQB=compute_spot_size(l,dB)
    while((iScale<11) and (dQB>=SPOT_SIZE_INFINITY/2)) #2km max
        dA=dB-2*(dB-dA); #look at left
        dQA=compute_spot_size(l,dA)

        if dQA<SPOT_SIZE_INFINITY/2:
            dB=dA; dQB=dQA
            break


        dC=dB-2*(dB-dC); # look at right
        dQC=compute_spot_size(l,dC)
        if dQC<SPOT_SIZE_INFINITY/2:
            dB=dC; dQB=dQC
            break

        iScale++


    if dQB>=SPOT_SIZE_INFINITY/2:
        return 0.; #unable to find a valid start point

    # find valid dA and dB and such as dA>=dB and dC>=dB
    dA=dB-1000.
    dC=dB+1000.
    dQA=compute_spot_size(l,dA)
    dQC=compute_spot_size(l,dC)

    #1st part: expand A and C, B became a minimum
    iScale=0
    while((iScale<11) and (dQA<=dQB)) #2km max
        dA=dB-2*(dB-dA)
        dQA=compute_spot_size(l,dA)
        iScale++


    iScale=0
    while((iScale<11) and (dQC<=dQB)) #2km max
        dC=dB-2*(dB-dC)
        dQC=compute_spot_size(l,dC)
        iScale++


    #exit if no solution
    if  (dQA<=dQB) or (dQC<=dQB) :
        return 0.

    # reduce interval around solution using dichotomy 1.5
    while (dC-dA>dZStep)
        double dAB=(dA+dB)*0.5
        double dQAB=compute_spot_size(l,dAB)

        if dQAB<dQB:
            # a minimum
            dC=dB
            dB=dAB
            dQB=dQAB

        else:
            double dBC=(dB+dC)*0.5
            double dQBC=compute_spot_size(l,dBC)

            if dQBC<dQB:
                # a minimum
                dA=dB
                dB=dBC
                dQB=dQBC

            else:
                # minimum is B
                dA=dAB
                dC=dBC




    # the minimum is at dB

    # TODO check all photon are ok at dB, return False; or use return value of compute_size
    # TODO use golden search

    return dB

#######################################
def compute_spot_size(self, l, z):
    # compute the intersection of all valid photon with plane at (0,0,z) with normal (1,0,0)

    # TODO add 45deg (or more) calliper

    # TODO add early abort tests on previous spot size
    double xMax=-1.e99
    double xMin=1.e99

    double yMax=-1.e99
    double yMin=1.e99
    bool bOneFound=False

	for(it = l.photons().begin();it<l.photons().end();++it)
	     Photon& p=*it
		if not p.is_valid():
            continue

        if p.dz==0.:
            continue; #TODO check clean error

        #compute t at z
        double t=(z-p.z)/p.dz

        if t<=0.:
           return SPOT_SIZE_INFINITY; #all photons must be used in autofocus, now

        double x=p.x+t*p.dx
        double y=p.y+t*p.dy

        if not bOneFound:
            xMax=xMin=x
            yMax=yMin=y

            bOneFound=True
            continue


		xMax = std.max(xMax, x)
		xMin = std.min(xMin, x)
		
		yMax = std.max(yMax, y)
		yMin = std.min(yMin, y)


    if not bOneFound:
        return SPOT_SIZE_INFINITY

    _xCenter=(xMax+xMin)*0.5
    _yCenter=(yMax+yMin)*0.5

    if xMax-xMin>yMax-yMin:
        return xMax-xMin
    else:
        return yMax-yMin

#######################################
def get_center(self, xCenter, yCenter):
    xCenter=_xCenter
    yCenter=_yCenter

#######################################
