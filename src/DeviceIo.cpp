// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <sstream>
using namespace std;

#include "DeviceIo.h"
#include "OpticalDevice.h"

#include "Properties.h"

////////////////////////////////////////////////////////////////////////////////
bool DeviceIo::save(string sFile, OpticalDevice* pOD)
{
    Properties prop;
    int iSurfclone;
    double dGainclone;

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
            if(pOD->get_clone(iS,THICK,iSurfclone,dGainclone))
            {
                prop.set(sSurfName+".thick.clone",iSurfclone);
                prop.set(sSurfName+".thick.clone.gain",dGainclone);
            }
        }
        else
        {
            double dZ=pOD->get(iS,Z);
            prop.set(sSurfName+".z",dZ);
            if(pOD->get_clone(iS,Z,iSurfclone,dGainclone))
            {
                prop.set(sSurfName+".z.clone",iSurfclone);
                prop.set(sSurfName+".z.clone.gain",dGainclone);
            }
        }

        if(pOD->get_autofocus() && (iS==pOD->nb_surface()-1))
            prop.set(sSurfName+".z.autofocus",true);

        //save diameter
        double dDiameter=pOD->get(iS,DIAMETER);
        prop.set(sSurfName+".diameter",dDiameter);
        if(pOD->get(iS,AUTO_DIAMETER)!=0.)
            prop.set(sSurfName+".diameter.auto",true);
        if(pOD->get_clone(iS,DIAMETER,iSurfclone,dGainclone))
        {
            prop.set(sSurfName+".diameter.clone",iSurfclone);
            prop.set(sSurfName+".diameter.clone.gain",dGainclone);
        }

        //save inner diameter
        double dInnerDiameter=pOD->get(iS,INNER_DIAMETER);
        if(dInnerDiameter!=0.)
        {
            prop.set(sSurfName+".inner_diameter",dInnerDiameter);
            if(pOD->get(iS,AUTO_INNER_DIAMETER)!=0.)
                prop.set(sSurfName+".inner_diameter.auto",true);
        }
        if(pOD->get_clone(iS,INNER_DIAMETER,iSurfclone,dGainclone))
        {
            prop.set(sSurfName+".inner_diameter.clone",iSurfclone);
            prop.set(sSurfName+".inner_diameter.clone.gain",dGainclone);
        }

        // save radius curvature
        double dRC=pOD->get(iS,RADIUS_CURVATURE);
        if(dRC>=RADIUS_CURVATURE_INFINITY/2)
            prop.set(sSurfName+".radius_curvature",string("inf"));
        else
            prop.set(sSurfName+".radius_curvature",dRC);

        if(pOD->get_clone(iS,RADIUS_CURVATURE,iSurfclone,dGainclone))
        {
            prop.set(sSurfName+".radius_curvature.clone",iSurfclone);
            prop.set(sSurfName+".radius_curvature.clone.gain",dGainclone);
        }

        //save conic
        double dConic=pOD->get(iS,CONIC);
        if(dConic!=0.)
            prop.set(sSurfName+".conic",dConic);
        if(pOD->get_clone(iS,CONIC,iSurfclone,dGainclone)) // TODO dgainclone!=1
            prop.set(sSurfName+".conic.clone",iSurfclone);

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

    prop.set("image.autofocus",pOD->get_autofocus());
    prop.set("image.autocurvature",pOD->get_image_autocurvature());

    // prop.set("half_field_of_view",pOD->half_field_of_view());
    prop.set("light.half_field_of_view",pOD->half_field_of_view());
    prop.set("light.colors",pOD->light_colors());
    prop.set("light.nbsteps",pOD->nb_intermediate_angles());

    //save commentary
    string sNote=pOD->note();
    if(!sNote.empty())
    {
        prop.set("note",sNote);
        prop.set("device.note",sNote);
    }

    //   prop.set("device.convention",pOD->relative_convention()?string("relative"):string("absolute"));
    prop.set("device.relative_convention",pOD->relative_convention());

    //save other parameters
    auto otherParams=pOD->all_parameters();
    for(auto it=otherParams.begin();it!=otherParams.end();++it)
        if(!(it->second.empty()))
            prop.set("parameter."+it->first,it->second);

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
    if(prop.exist("device.relative_convention"))
        pOD->set_relative_convention(prop.get_bool("device.relative_convention"));

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
            sType="image"; //conversion from old format

        pOD->insert_surface(iS);
        pOD->set_type(iS,sType);

        if (prop.exist(sSurfName+".diameter"))
        {
            double dDiameter=prop.get_double(sSurfName+".diameter");
            pOD->set(iS,DIAMETER,dDiameter);

            double dGainclone=1;
            if(prop.exist(sSurfName+".diameter.clone.gain"))
                dGainclone=prop.get_int(sSurfName+".diameter.clone.gain");
            if(prop.exist(sSurfName+".diameter.clone"))
                pOD->set_clone(iS,DIAMETER,prop.get_int(sSurfName+".diameter.clone"),dGainclone);
        }
        if (prop.exist(sSurfName+".diameter.auto"))
            pOD->set(iS,AUTO_DIAMETER,prop.get_bool(sSurfName+".diameter.auto"));

        if (prop.exist(sSurfName+".inner_diameter"))
        {
            double dInnerDiameter=prop.get_double(sSurfName+".inner_diameter");
            pOD->set(iS,INNER_DIAMETER,dInnerDiameter);

            double dGainclone=1;
            if(prop.exist(sSurfName+".inner_diameter.clone.gain"))
                dGainclone=prop.get_int(sSurfName+".inner_diameter.clone.gain");
            if(prop.exist(sSurfName+".inner_diameter.clone"))
                pOD->set_clone(iS,INNER_DIAMETER,prop.get_int(sSurfName+".inner_diameter.clone"),dGainclone);
        }
        if (prop.exist(sSurfName+".inner_diameter.auto"))
            pOD->set(iS,AUTO_INNER_DIAMETER,prop.get_bool(sSurfName+".inner_diameter.auto"));

        if (prop.exist(sSurfName+".radius_curvature"))
        {
            if(prop.get(sSurfName+".radius_curvature")=="inf")
                pOD->set(iS,RADIUS_CURVATURE,RADIUS_CURVATURE_INFINITY);
            else
                pOD->set(iS,RADIUS_CURVATURE,prop.get_double(sSurfName+".radius_curvature"));

            double dGainclone=1.;
            if(prop.exist(sSurfName+".radius_curvature.clone.gain"))
                dGainclone=prop.get_int(sSurfName+".radius_curvature.clone.gain");
            if(prop.exist(sSurfName+".radius_curvature.clone"))
                pOD->set_clone(iS,RADIUS_CURVATURE,prop.get_int(sSurfName+".radius_curvature.clone"),dGainclone);
        }

        if (prop.exist(sSurfName+".conic"))
        {
            pOD->set(iS,CONIC,prop.get_double(sSurfName+".conic"));
            double dGainclone=1;
            if(prop.exist(sSurfName+".conic.clone.gain"))
                dGainclone=prop.get_int(sSurfName+".conic.clone.gain");
            if(prop.exist(sSurfName+".conic.clone"))
                pOD->set_clone(iS,CONIC,prop.get_int(sSurfName+".conic.clone"),dGainclone);
        }

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
            double dGainclone=1;
            if(prop.exist(sSurfName+".z.clone.gain"))
                dGainclone=prop.get_int(sSurfName+".z.clone.gain");
            if(prop.exist(sSurfName+".z.clone"))
                pOD->set_clone(iS,Z,prop.get_int(sSurfName+".z.clone"),dGainclone);
        }

        if (prop.exist(sSurfName+".thick"))
        {
            double dThick=prop.get_double(sSurfName+".thick");
            pOD->set(iS,THICK,dThick);
            double dGainclone=1;
            if(prop.exist(sSurfName+".thick.clone.gain"))
                dGainclone=prop.get_int(sSurfName+".thick.clone.gain");
            if(prop.exist(sSurfName+".thick.clone"))
                pOD->set_clone(iS,THICK,prop.get_int(sSurfName+".thick.clone"),dGainclone);
        }

        //old format
        if (prop.exist(sSurfName+".z.autofocus"))
            pOD->set_autofocus(prop.get_bool(sSurfName+".z.autofocus"));

        //new_format
        if (prop.exist("image.autofocus"))
            pOD->set_autofocus(prop.get_bool("image.autofocus"));

        if (prop.exist("image.autocurvature"))
            pOD->set_image_autocurvature(prop.get_bool("image.autocurvature"));

        iS++;
        stringstream ss2;
        ss2 << iS;
        ss2 >> sSurfName;
    }
    while (prop.exist(sSurfName+".kind") || prop.exist(sSurfName+".type")); // comptatibility mode

    //old format
    if (prop.exist("half_field_of_view"))
        pOD->set_half_field_of_view(prop.get_double("half_field_of_view"));

    // new format
    if (prop.exist("light.half_field_of_view"))
        pOD->set_half_field_of_view(prop.get_double("light.half_field_of_view"));

    if (prop.exist("light.nbsteps"))
        pOD->set_nb_intermediate_angles(prop.get_int("light.nbsteps"));
    else
        pOD->set_nb_intermediate_angles(3);

    if (prop.exist("light.colors"))
        pOD->set_light_colors(prop.get("light.colors"));
    else
        pOD->set_light_colors(""); //temporary

    //old format
    //read comment
    if(prop.exist("note"))
    {
        string sNote=prop.get("note");
        pOD->set_note(sNote);
    }

    //new format
    if(prop.exist("device.note"))
    {
        string sNote=prop.get("device.note");
        pOD->set_note(sNote);
    }

    //load other parameters
    auto otherParams=prop.all();
    for(auto it=otherParams.begin();it!=otherParams.end();++it)
    {
        string sKey=it->first;
        if(sKey.size()>10)
        {
            string sSubKey=sKey.substr(0,10);
            if(sSubKey=="parameter.")
                pOD->set_parameter(sKey.substr(10,string::npos),it->second);
        }
    }

    return pOD;
}
////////////////////////////////////////////////////////////////////////////////
