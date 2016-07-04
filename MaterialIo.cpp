// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialIo.h"
#include "Properties.h"

#include "GlassSellmeier.h"

//////////////////////////////////////////////////////////////////////////////
Glass* MaterialIo::load(string sFile)
{
    Properties p;
    if(p.load(sFile)==false)
        return 0;

    string sName=p.get("Name");
    string sFormulae=p.get("Formulae");
    int iSolidColor=p.get_int("SolidColor");

    if(sFormulae=="Sellmeier")
    {
        double B1=p.get_double("B1");
        double B2=p.get_double("B2");
        double B3=p.get_double("B3");
        double C1=p.get_double("C1");
        double C2=p.get_double("C2");
        double C3=p.get_double("C3");

        GlassSellmeier* m=new GlassSellmeier;
        m->set_name(sName);
/*
        m->_B1=B1;
        m->_B2=B2;
        m->_B3=B3;
        m->_C1=C1;
        m->_C2=C2;
        m->_C3=C3;

        m->set_solid_color(iSolidColor);
*/
        return m;
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////////
