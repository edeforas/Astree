// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceScaling.h"
#include "OpticalDevice.h"

#include <cassert>

////////////////////////////////////////////////////////////////////////////////
bool DeviceScaling::scale(OpticalDevice *pDevice, double dRatio, bool bScaleDiameter, bool bScaleFocal)
{
    assert(pDevice!=0);

    if(dRatio<=0.)
        return false;

    bool bConvention=pDevice->relative_convention();
    pDevice->set_relative_convention(true);

    for(int iS=0;iS<pDevice->nb_surface();iS++)
    {
        if(bScaleDiameter)
        {
            if(pDevice->is_clone(iS,DIAMETER)==false)
            {
                double dDiameter=pDevice->get(iS,DIAMETER,false);
                pDevice->set(iS,DIAMETER,dDiameter*dRatio);
            }

            if(pDevice->is_clone(iS,INNER_DIAMETER)==false)
            {
                double dInnerDiameter=pDevice->get(iS,INNER_DIAMETER,false);
                pDevice->set(iS,INNER_DIAMETER,dInnerDiameter*dRatio);
            }
        }

        if(bScaleFocal)
        {
            if(pDevice->is_clone(iS,THICK)==false) //always in relative convention
            {
                double dTHICK=pDevice->get(iS,THICK,false);
                pDevice->set(iS,THICK,dTHICK*dRatio);
            }

            if(pDevice->is_clone(iS,RADIUS_CURVATURE)==false)
            {
                double dRadiusCurvature=pDevice->get(iS,RADIUS_CURVATURE,false);
                pDevice->set(iS,RADIUS_CURVATURE,dRadiusCurvature*dRatio);
            }

            double dRatio2=dRatio*dRatio;
            double dRatio3=dRatio2*dRatio;
            double dRatio5=dRatio2*dRatio3;
            double dRatio7=dRatio2*dRatio5;
            double dRatio9=dRatio2*dRatio7;

            if(pDevice->is_clone(iS,R4)==false)
            {
                double dR4=pDevice->get(iS,R4,false);
                pDevice->set(iS,R4,dR4/dRatio3);
            }

            if(pDevice->is_clone(iS,R6)==false)
            {
                double dR6=pDevice->get(iS,R6,false);
                pDevice->set(iS,R6,dR6/dRatio5);
            }

            if(pDevice->is_clone(iS,R8)==false)
            {
                double dR8=pDevice->get(iS,R8,false);
                pDevice->set(iS,R8,dR8/dRatio7);
            }

            if(pDevice->is_clone(iS,R10)==false)
            {
                double dR10=pDevice->get(iS,R10,false);
                pDevice->set(iS,R10,dR10/dRatio9);
            }
        }
    }

    pDevice->set_relative_convention(bConvention);
    return true;
}
//////////////////////////////////////////////////////////////////////////////
