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
    Properties prop;

    for (int iS=0;iS<pOD->nb_surface();iS++)
    {
        stringstream ss;
        string sSurfName;
        ss << iS;
        ss >> sSurfName;

        prop.set(sSurfName+".type",pOD->type(iS));

        if(pOD->relative_convention())
        {
            double dThick=pOD->get(iS,THICK);
            prop.set(sSurfName+".thick",dThick);
        }
        else
        {
            double dZ=pOD->get(iS,Z);
            prop.set(sSurfName+".z",dZ);
        }

        if(pOD->get_autofocus() && (iS==pOD->nb_surface()-1))
            prop.set(sSurfName+".z.autofocus",true);

        double dDiameter=pOD->get(iS,DIAMETER);
        prop.set(sSurfName+".diameter",dDiameter);
        if(pOD->get(iS,AUTO_DIAMETER))
            prop.set(sSurfName+".diameter.auto",true);

        double dInnerDiameter=pOD->get(iS,INNER_DIAMETER);
        if(dInnerDiameter!=0.)
        {
            prop.set(sSurfName+".inner_diameter",dInnerDiameter);
            if(pOD->get(iS,AUTO_INNER_DIAMETER))
                prop.set(sSurfName+".inner_diameter.auto",true);
        }

        double dRC=pOD->get(iS,RADIUS_CURVATURE);
        if(dRC>=RADIUS_CURVATURE_INFINITY/2)
            prop.set(sSurfName+".radius_curvature",string("inf"));
        else
            prop.set(sSurfName+".radius_curvature",dRC);

        double dConic=pOD->get(iS,CONIC);
        if(dConic!=0.)
            prop.set(sSurfName+".conic",dConic);

        double dR4=pOD->get(iS,R4);
        if(dR4!=0.)
            prop.set(sSurfName+".r4",dR4);

        double dR6=pOD->get(iS,R6);
        if(dR6!=0.)
            prop.set(sSurfName+".r6",dR6);

        double dR8=pOD->get(iS,R8);
        if(dR8!=0.)
            prop.set(sSurfName+".r8",dR8);

        double dR10=pOD->get(iS,R10);
        if(dR10!=0.)
            prop.set(sSurfName+".r10",dR10);

        if (pOD->comment(iS).empty()==false)
            prop.set(sSurfName+".comment",pOD->comment(iS));
    }

    //prop.set("autofocus",pOD->get_autofocus());

    prop.set("half_field_of_view",pOD->half_field_of_view());
    prop.set("light.colors",pOD->light_colors());
    prop.set("light.nbsteps",pOD->nb_intermediate_angles());

    //save commentary
    string sNote=pOD->note();
    if(!sNote.empty())
        prop.set("note",sNote);

    prop.set("device.convention",pOD->relative_convention()?string("relative"):string("absolute"));
    //prop.set("relative_convention",pOD->relative_convention());

    return prop.save(sFile);
}
////////////////////////////////////////////////////////////////////////////////
OpticalDevice* DeviceIo::load(string sFile)
{
    Properties prop;
    if (!prop.load(sFile))
        return 0;

    OpticalDevice* pOD=new OpticalDevice();
    if(prop.exist("device.convention"))
        pOD->set_relative_convention(prop.get("device.convention")=="relative");
    else
        pOD->set_relative_convention(false);

    //new way
 //   if(prop.exist("relative_convention"))
 //       pOD->set_relative_convention(prop.get_bool("relative_convention"));

    int iS=0;
    string sSurfName="0";
    do
    {
        string sType;
        if(prop.exist(sSurfName+".type"))
            sType=prop.get(sSurfName+".type");

        if(sType.empty())
            break;

        if(sType=="observe")
            sType="image";

        pOD->insert_surface(iS);
        pOD->set_type(iS,sType);

        if (prop.exist(sSurfName+".diameter"))
        {
            double dDiameter=prop.get_double(sSurfName+".diameter");
            pOD->set(iS,DIAMETER,dDiameter);
        }
        if (prop.exist(sSurfName+".diameter.auto"))
            pOD->set(iS,AUTO_DIAMETER,prop.get_bool(sSurfName+".diameter.auto"));

        if (prop.exist(sSurfName+".inner_diameter"))
        {
            double dInnerDiameter=prop.get_double(sSurfName+".inner_diameter");
            pOD->set(iS,INNER_DIAMETER,dInnerDiameter);
        }
        if (prop.exist(sSurfName+".inner_diameter.auto"))
            pOD->set(iS,AUTO_INNER_DIAMETER,prop.get_bool(sSurfName+".inner_diameter.auto"));


        if (prop.exist(sSurfName+".radius_curvature"))
        {
            if(prop.get(sSurfName+".radius_curvature")=="inf")
                pOD->set(iS,RADIUS_CURVATURE,RADIUS_CURVATURE_INFINITY);
            else
                pOD->set(iS,RADIUS_CURVATURE,prop.get_double(sSurfName+".radius_curvature"));
        }

        if (prop.exist(sSurfName+".conic"))
            pOD->set(iS,CONIC,prop.get_double(sSurfName+".conic"));

        if (prop.exist(sSurfName+".r4"))
            pOD->set(iS,R4,prop.get_double(sSurfName+".r4"));

        if (prop.exist(sSurfName+".r6"))
            pOD->set(iS,R6,prop.get_double(sSurfName+".r6"));

        if (prop.exist(sSurfName+".r8"))
            pOD->set(iS,R8,prop.get_double(sSurfName+".r8"));

        if (prop.exist(sSurfName+".r10"))
            pOD->set(iS,R10,prop.get_double(sSurfName+".r10"));

        if (prop.exist(sSurfName+".comment"))
        {
            pOD->set_comment(iS,prop.get(sSurfName+".comment"));
        }

        if (prop.exist(sSurfName+".z"))
        {
            double dZ=prop.get_double(sSurfName+".z");
            pOD->set(iS,Z,dZ);
        }

        if (prop.exist(sSurfName+".thick"))
        {
            double dThick=prop.get_double(sSurfName+".thick");
            pOD->set(iS,THICK,dThick);
        }

        //old format
        if (prop.exist(sSurfName+".z.autofocus"))
        {
            pOD->set_autofocus(prop.get_bool(sSurfName+".z.autofocus"));
        }

        //new_format
        if (prop.exist("autofocus"))
        {
            pOD->set_autofocus(prop.get_bool("autofocus"));
        }

        iS++;
        stringstream ss2;
        ss2 << iS;
        ss2 >> sSurfName;
    }
    while (prop.exist(sSurfName+".kind") || prop.exist(sSurfName+".type")); // comptatibility mode

    if (prop.exist("half_field_of_view"))
        pOD->set_half_field_of_view(prop.get_double("half_field_of_view"));

    if (prop.exist("light.nbsteps"))
        pOD->set_nb_intermediate_angles(prop.get_double("light.nbsteps"));
    else
        pOD->set_nb_intermediate_angles(3);

    if (prop.exist("light.colors"))
    {
        pOD->set_light_colors(prop.get("light.colors"));
    }
    else
        pOD->set_light_colors(""); //temporaire

    //lit le commentaire
    if(prop.exist("note"))
    {
        string sNote=prop.get("note");
        pOD->set_note(sNote);
    }

    return pOD;
}
////////////////////////////////////////////////////////////////////////////////
