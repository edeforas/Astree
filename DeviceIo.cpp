// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <sstream>
using namespace std;

#include "DeviceIo.h"
#include "OpticalDevice.h"

#include "Properties.h"

////////////////////////////////////////////////////////////////////////////////
bool DeviceIo::save(string sFile,OpticalDevice* pD)
{
    Properties ac;

    for (int iS=0;iS<pD->nb_surface();iS++)
    {
        stringstream ss;
        string sSurfName;
        ss << iS;
        ss >> sSurfName;

        ac.set(sSurfName+".type",pD->type(iS));
        /*
        double dX;
        pD->get(iS,"x",dX);
        ac.set(sSurfName+".x",dX);

        double dY;
        pD->get(iS,"y",dY);
        ac.set(sSurfName+".y",dY);
*/
        double dZ=pD->z(iS);
        ac.set(sSurfName+".z",dZ);
        if(pD->get_autofocus((iS)))
            ac.set(sSurfName+".z.autofocus",true);

        double dDiameter=pD->diameter(iS);
        ac.set(sSurfName+".diameter",dDiameter);
        if(pD->get_auto_diameter(iS))
            ac.set(sSurfName+".diameter.auto",true);

        double dInnerDiameter=pD->inner_diameter(iS);
        if(dInnerDiameter!=0.)
        {
            ac.set(sSurfName+".inner_diameter",dInnerDiameter);
            if(pD->get_auto_inner_diameter(iS))
                ac.set(sSurfName+".inner_diameter.auto",true);
        }

        double dRC=pD->radius_curvature(iS);
        if(dRC>=RADIUS_CURVATURE_INFINITY/2)
            ac.set(sSurfName+".radius_curvature",string("inf"));
        else
            ac.set(sSurfName+".radius_curvature",dRC);

        if(pD->conic(iS)!=0.)
            ac.set(sSurfName+".conic",pD->conic(iS));

        double dR4=pD->get(iS,R4);
        if(dR4!=0.)
            ac.set(sSurfName+".r4",dR4);

        double dR6=pD->get(iS,R6);
        if(dR6!=0.)
            ac.set(sSurfName+".r6",dR6);

        double dR8=pD->get(iS,R8);
        if(dR8!=0.)
            ac.set(sSurfName+".r8",dR8);

        double dR10=pD->get(iS,R10);
        if(dR10!=0.)
            ac.set(sSurfName+".r10",dR10);

        if (pD->comment(iS).empty()==false)
            ac.set(sSurfName+".comment",pD->comment(iS));
    }

    ac.set("half_field_of_view",pD->half_field_of_view());
    ac.set("light.colors",pD->light_colors());
    ac.set("light.nbsteps",pD->nb_intermediate_angles());

    //sauve le commentaire
    string sNote=pD->note();
    if(!sNote.empty())
        ac.set("note",sNote);

    ac.set("device.convention",pD->convention());

    return ac.save(sFile);
}
////////////////////////////////////////////////////////////////////////////////
OpticalDevice* DeviceIo::load(string sFile)
{
    Properties ac;
    if (!ac.load(sFile))
        return 0;

    OpticalDevice* pD=new OpticalDevice();
    if(ac.exist("device.convention"))
        pD->set_convention(ac.get("device.convention"));
    else
        pD->set_convention("absolute");


    int iS=0;
    string sSurfName="0";
    do
    {
        string sType;
        if(ac.exist(sSurfName+".type"))
            sType=ac.get(sSurfName+".type");

        if(sType.empty())
            break;

        if(sType=="observe")
            sType="image";

        pD->insert_surface(iS);
        pD->set_type(iS,sType);

        if (ac.exist(sSurfName+".diameter"))
        {
            double dDiameter=ac.get_double(sSurfName+".diameter");
            pD->set_diameter(iS,dDiameter);
        }
        if (ac.exist(sSurfName+".diameter.auto"))
            pD->set_auto_diameter(iS,ac.get_bool(sSurfName+".diameter.auto"));

        if (ac.exist(sSurfName+".inner_diameter"))
        {
            double dInnerDiameter=ac.get_double(sSurfName+".inner_diameter");
            pD->set_inner_diameter(iS,dInnerDiameter);
        }
        if (ac.exist(sSurfName+".inner_diameter.auto"))
            pD->set_auto_inner_diameter(iS,ac.get_bool(sSurfName+".inner_diameter.auto"));


        if (ac.exist(sSurfName+".radius_curvature"))
        {
            if(ac.get(sSurfName+".radius_curvature")=="inf")
                pD->set_radius_curvature(iS,RADIUS_CURVATURE_INFINITY);
            else
                pD->set_radius_curvature(iS,ac.get_double(sSurfName+".radius_curvature"));
        }

        if (ac.exist(sSurfName+".conic"))
            pD->set_conic(iS,ac.get_double(sSurfName+".conic"));

        double dR4=0.;
        if (ac.exist(sSurfName+".r4"))
            dR4=ac.get_double(sSurfName+".r4");

        double dR6=0.;
        if (ac.exist(sSurfName+".r6"))
            dR6=ac.get_double(sSurfName+".r6");

        double dR8=0.;
        if (ac.exist(sSurfName+".r8"))
            dR8=ac.get_double(sSurfName+".r8");

        double dR10=0.;
        if (ac.exist(sSurfName+".r10"))
            dR10=ac.get_double(sSurfName+".r10");

        pD->set_poly_aspheric(iS,dR4,dR6,dR8,dR10);

        if (ac.exist(sSurfName+".comment"))
        {
            pD->set_comment(iS,ac.get(sSurfName+".comment"));
        }

        if (ac.exist(sSurfName+".z"))
        {
            double dZ=ac.get_double(sSurfName+".z");
            pD->set_z(iS,dZ);
        }

        if (ac.exist(sSurfName+".z.autofocus"))
        {
            pD->set_autofocus(iS,ac.get_bool(sSurfName+".z.autofocus"));
        }

        iS++;
        stringstream ss2;
        ss2 << iS;
        ss2 >> sSurfName;
    }
    while (ac.exist(sSurfName+".kind") || ac.exist(sSurfName+".type")); // comptatibility mode

    if (ac.exist("half_field_of_view"))
        pD->set_half_field_of_view(ac.get_double("half_field_of_view"));

    if (ac.exist("light.nbsteps"))
        pD->set_nb_intermediate_angles(ac.get_double("light.nbsteps"));
    else
        pD->set_nb_intermediate_angles(3);

    if (ac.exist("light.colors"))
    {
        pD->set_light_colors(ac.get("light.colors"));
    }
    else
        pD->set_light_colors(""); //temporaire

    //lit le commentaire
    if(ac.exist("note"))
    {
        string sNote=ac.get("note");
        pD->set_note(sNote);
    }

    return pD;
}
////////////////////////////////////////////////////////////////////////////////
