// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <sstream>
using namespace std;

#include "DeviceIo.h"
#include "OpticalDevice.h"

#include "Properties.h"

////////////////////////////////////////////////////////////////////////////////
bool DeviceIo::save(string sFile,OpticalDevice* pOD)
{
    Properties ac;

    for (int iS=0;iS<pOD->nb_surface();iS++)
    {
        stringstream ss;
        string sSurfName;
        ss << iS;
        ss >> sSurfName;

        ac.set(sSurfName+".type",pOD->type(iS));

        if(pOD->relative_convention())
        {
            double dThick=pOD->get(iS,THICK);
            ac.set(sSurfName+".thick",dThick);
        }
        else
        {
            double dZ=pOD->get(iS,Z);
            ac.set(sSurfName+".z",dZ);
        }

        if(pOD->get_autofocus((iS)))
            ac.set(sSurfName+".z.autofocus",true);

        double dDiameter=pOD->get(iS,DIAMETER);
        ac.set(sSurfName+".diameter",dDiameter);
        if(pOD->get(iS,AUTO_DIAMETER))
            ac.set(sSurfName+".diameter.auto",true);

        double dInnerDiameter=pOD->get(iS,INNER_DIAMETER);
        if(dInnerDiameter!=0.)
        {
            ac.set(sSurfName+".inner_diameter",dInnerDiameter);
            if(pOD->get(iS,AUTO_INNER_DIAMETER))
                ac.set(sSurfName+".inner_diameter.auto",true);
        }

        double dRC=pOD->get(iS,RADIUS_CURVATURE);
        if(dRC>=RADIUS_CURVATURE_INFINITY/2)
            ac.set(sSurfName+".radius_curvature",string("inf"));
        else
            ac.set(sSurfName+".radius_curvature",dRC);

        double dConic=pOD->get(iS,CONIC);
        if(dConic!=0.)
            ac.set(sSurfName+".conic",dConic);

        double dR4=pOD->get(iS,R4);
        if(dR4!=0.)
            ac.set(sSurfName+".r4",dR4);

        double dR6=pOD->get(iS,R6);
        if(dR6!=0.)
            ac.set(sSurfName+".r6",dR6);

        double dR8=pOD->get(iS,R8);
        if(dR8!=0.)
            ac.set(sSurfName+".r8",dR8);

        double dR10=pOD->get(iS,R10);
        if(dR10!=0.)
            ac.set(sSurfName+".r10",dR10);

        if (pOD->comment(iS).empty()==false)
            ac.set(sSurfName+".comment",pOD->comment(iS));
    }

    ac.set("half_field_of_view",pOD->half_field_of_view());
    ac.set("light.colors",pOD->light_colors());
    ac.set("light.nbsteps",pOD->nb_intermediate_angles());

    //save commentary
    string sNote=pOD->note();
    if(!sNote.empty())
        ac.set("note",sNote);

    ac.set("device.convention",pOD->relative_convention()?string("relative"):string("absolute"));

    return ac.save(sFile);
}
////////////////////////////////////////////////////////////////////////////////
OpticalDevice* DeviceIo::load(string sFile)
{
    Properties ac;
    if (!ac.load(sFile))
        return 0;

    OpticalDevice* pOD=new OpticalDevice();
    if(ac.exist("device.convention"))
        pOD->set_relative_convention(ac.get("device.convention")=="relative");
    else
        pOD->set_relative_convention(false);

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

        pOD->insert_surface(iS);
        pOD->set_type(iS,sType);

        if (ac.exist(sSurfName+".diameter"))
        {
            double dDiameter=ac.get_double(sSurfName+".diameter");
            pOD->set(iS,DIAMETER,dDiameter);
        }
        if (ac.exist(sSurfName+".diameter.auto"))
            pOD->set(iS,AUTO_DIAMETER,ac.get_bool(sSurfName+".diameter.auto"));

        if (ac.exist(sSurfName+".inner_diameter"))
        {
            double dInnerDiameter=ac.get_double(sSurfName+".inner_diameter");
            pOD->set(iS,INNER_DIAMETER,dInnerDiameter);
        }
        if (ac.exist(sSurfName+".inner_diameter.auto"))
            pOD->set(iS,AUTO_INNER_DIAMETER,ac.get_bool(sSurfName+".inner_diameter.auto"));


        if (ac.exist(sSurfName+".radius_curvature"))
        {
            if(ac.get(sSurfName+".radius_curvature")=="inf")
                pOD->set(iS,RADIUS_CURVATURE,RADIUS_CURVATURE_INFINITY);
            else
                pOD->set(iS,RADIUS_CURVATURE,ac.get_double(sSurfName+".radius_curvature"));
        }

        if (ac.exist(sSurfName+".conic"))
            pOD->set(iS,CONIC,ac.get_double(sSurfName+".conic"));

        if (ac.exist(sSurfName+".r4"))
            pOD->set(iS,R4,ac.get_double(sSurfName+".r4"));

        if (ac.exist(sSurfName+".r6"))
            pOD->set(iS,R6,ac.get_double(sSurfName+".r6"));

        if (ac.exist(sSurfName+".r8"))
            pOD->set(iS,R8,ac.get_double(sSurfName+".r8"));

        if (ac.exist(sSurfName+".r10"))
            pOD->set(iS,R10,ac.get_double(sSurfName+".r10"));

        if (ac.exist(sSurfName+".comment"))
        {
            pOD->set_comment(iS,ac.get(sSurfName+".comment"));
        }

        if (ac.exist(sSurfName+".z"))
        {
            double dZ=ac.get_double(sSurfName+".z");
            pOD->set(iS,Z,dZ);
        }

        if (ac.exist(sSurfName+".thick"))
        {
            double dThick=ac.get_double(sSurfName+".thick");
            pOD->set(iS,THICK,dThick);
        }

        if (ac.exist(sSurfName+".z.autofocus"))
        {
            pOD->set_autofocus(iS,ac.get_bool(sSurfName+".z.autofocus"));
        }

        iS++;
        stringstream ss2;
        ss2 << iS;
        ss2 >> sSurfName;
    }
    while (ac.exist(sSurfName+".kind") || ac.exist(sSurfName+".type")); // comptatibility mode

    if (ac.exist("half_field_of_view"))
        pOD->set_half_field_of_view(ac.get_double("half_field_of_view"));

    if (ac.exist("light.nbsteps"))
        pOD->set_nb_intermediate_angles(ac.get_double("light.nbsteps"));
    else
        pOD->set_nb_intermediate_angles(3);

    if (ac.exist("light.colors"))
    {
        pOD->set_light_colors(ac.get("light.colors"));
    }
    else
        pOD->set_light_colors(""); //temporaire

    //lit le commentaire
    if(ac.exist("note"))
    {
        string sNote=ac.get("note");
        pOD->set_note(sNote);
    }

    return pOD;
}
////////////////////////////////////////////////////////////////////////////////
