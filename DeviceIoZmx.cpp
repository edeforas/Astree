// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <sstream>
#include <fstream>
#include <string>
using namespace std;

#include "DeviceIoZmx.h"
#include "OpticalDevice.h"

////////////////////////////////////////////////////////////////////////////////
OpticalDevice* DeviceIoZmx::import(string sFile)
{
    OpticalDevice* pD=new OpticalDevice();
    pD->set_convention("relative");

    ifstream f(sFile.c_str(),ios::in);

    double dDimensionFactor=1.;
    int iSurface=0;
    bool bSurfPending=false;
    string sType;
    double dCurvature=0;
    double dTick=0;
    double dDiameter=0;
    double dConic=0.;
    double dInnerDiameter=0.;
    string sComment;
    string sNote;

    while(!f.eof())
    {
        string sLine;
        ::getline(f,sLine);

        //trim first space if any
        unsigned int iPos=sLine.find_first_not_of(" ");
        if(iPos!=string::npos)
            sLine=sLine.substr(iPos);

        if(sLine.size()<4)
            continue; // nothing to parse

        string sKey=sLine.substr(0,4);

        string sVal=sLine.substr(4); // can be empty

        //trim first space if any
        unsigned int iPos2=sVal.find_first_not_of(" ");
        if(iPos2!=string::npos)
            sVal=sVal.substr(iPos2);

        if((sKey=="SURF") || (sKey=="BLNK"))
        {
            if(bSurfPending)
            {
                pD->insert_surface(iSurface);
                pD->set_type(iSurface,sType);
                pD->set_z(iSurface,dDimensionFactor*dTick);
                pD->set_radius_curvature(iSurface,dCurvature/dDimensionFactor);
                pD->set_diameter(iSurface,dDiameter*dDimensionFactor);
                pD->set_conic(iSurface,dConic);
                pD->set_comment(iSurface,sComment);
                pD->set_inner_diameter(iSurface,dInnerDiameter*dDimensionFactor);
                bSurfPending=false;
            }

            if(sKey=="SURF")
            {
                iSurface=-1;
                stringstream ss(sVal); ss >> iSurface;

                if(iSurface==-1)
                { delete pD; return 0; } // not a valid field

                bSurfPending=true;

                dCurvature=0;
                dDiameter=0;
                sType="void";
                dConic=0;
                sComment="";
                dInnerDiameter=0.;
            }
        }

        if(sKey=="UNIT")
        {
            string sFirstVal=sVal.substr(0,sVal.find_first_of(" "));

            if(sFirstVal=="MM")
                dDimensionFactor=1.;

            if(sFirstVal=="METER")
                dDimensionFactor=1.e3;

            if(sFirstVal=="CM")
                dDimensionFactor=10.;

            if(sFirstVal=="IN")
                dDimensionFactor=25.4;
        }

        if(sKey=="TYPE")
        {
            if( (sVal!="STANDARD") && (sVal!="EVENASPH") )
            { delete pD; return 0; } // dont' know how to parse
        }

        if(sKey=="CURV")
        {
            stringstream ss(sVal); ss >> dCurvature;
        }

        if(sKey=="COMM")
        {
            sComment=sVal;
        }

        if(sKey=="NOTE")
        {
            string sNoteVal;
            unsigned int iPos=sVal.find_first_of(" ");
            if(iPos!=string::npos)
                sNoteVal=sVal.substr(iPos,string::npos);

            sNote+=sNoteVal+"\n";
        }

        if(sKey=="DISZ")
        {
            stringstream ss(sVal); ss >> dTick;
        }

        if(sKey=="OBSC")
        {
            double dTemp;
            stringstream ss(sVal); ss >> dTemp >> dInnerDiameter;
        }

        if(sKey=="CONI")
        {
            stringstream ss(sVal); ss >> dConic;
        }


        if(sKey=="DIAM")
        {
            stringstream ss(sVal); ss >> dDiameter;
            dDiameter*=2;
        }

        if(sKey=="GLAS")
        {
            string sFirstWord;
            unsigned int iPos=sVal.find_first_of(" ");
            if(iPos!=string::npos)
                sFirstWord=sVal.substr(0,iPos);

            sType="BK7"; // TODO use glass catalog

            if(sFirstWord=="MIRROR")
                sType="reflect";

            if(sFirstWord=="BK7")
                sType=sVal;

            if(sFirstWord=="silica")
                sType=sVal;

            if(sFirstWord=="F2")
                sType=sVal;

            if(sFirstWord=="SF5")
                sType=sVal;
        }
    }

    if(!sNote.empty())
        pD->set_note(sNote);

    return pD;
}
////////////////////////////////////////////////////////////////////////////////
