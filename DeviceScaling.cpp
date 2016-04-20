// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceScaling.h"
#include "OpticalDevice.h"

#include <cassert>

////////////////////////////////////////////////////////////////////////////////
bool DeviceScaling::scale(OpticalDevice *pDevice,double dRatio)
{
    assert(pDevice!=0);

    if(dRatio<=0.)
        return false;

    for(int iS=0;iS<pDevice->nb_surface();iS++)
    {
        double dZ=pDevice->z(iS);
        double dDiameter=pDevice->diameter(iS);
        double dRadiusCurvature=pDevice->radius_curvature(iS);

        pDevice->set_z(iS,dZ*dRatio);
        pDevice->set_diameter(iS,dDiameter*dRatio);
        pDevice->set_radius_curvature(iS,dRadiusCurvature*dRatio);

        double dRatio2=dRatio*dRatio;
        double dRatio3=dRatio2*dRatio;
        double dRatio5=dRatio2*dRatio3;
        double dRatio7=dRatio2*dRatio5;
        double dRatio9=dRatio2*dRatio7;

        double dR4=pDevice->get(iS,R4);
        double dR6=pDevice->get(iS,R6);
        double dR8=pDevice->get(iS,R8);
        double dR10=pDevice->get(iS,R10);

        dR4=dR4/dRatio3;
        dR6=dR6/dRatio5;
        dR8=dR8/dRatio7;
        dR10=dR10/dRatio9;

        pDevice->set_poly_aspheric(iS,dR4,dR6,dR8,dR10);
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////////
