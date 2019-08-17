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
    //TODO test and reject if format=utf16

    OpticalDevice* pOD=new OpticalDevice();
    pOD->set_relative_convention(true);

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
    double dApertureDiameter=-1.;
    string sComment;
    string sNote;
    string sGlassCatalog;

    while(!f.eof())
    {
        string sLine;
        std::getline(f,sLine);

        //trim first space if any
        size_t iPos=sLine.find_first_not_of(" ");
        if(iPos!=string::npos)
            sLine=sLine.substr(iPos);

        if(sLine.size()<4)
            continue; // nothing to parse

        string sKey=sLine.substr(0,4);

        string sVal=sLine.substr(4); // can be empty

        //trim first space if any
        size_t iPos2=sVal.find_first_not_of(" ");
        if(iPos2!=string::npos)
            sVal=sVal.substr(iPos2);

        if((sKey=="SURF") || (sKey=="BLNK"))
        {
            if(bSurfPending)
            {
                pOD->insert_surface(iSurface);
                pOD->set_comment(iSurface,sComment);
                pOD->set_type(iSurface,sType);
                pOD->set(iSurface,THICK,dDimensionFactor*dTick);
                pOD->set(iSurface,CURVATURE,dCurvature/dDimensionFactor);
                pOD->set(iSurface,DIAMETER,dDiameter*dDimensionFactor);
                pOD->set(iSurface,CONIC,dConic);
                pOD->set(iSurface,INNER_DIAMETER,dInnerDiameter*dDimensionFactor);
                bSurfPending=false;
            }

            if(sKey=="SURF")
            {
                iSurface=-1;
                stringstream ss(sVal); ss >> iSurface;

                if(iSurface==-1)
                { delete pOD; return 0; } // not a valid field

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
            { delete pOD; return 0; } // dont' know how to parse
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
            auto iPos=sVal.find_first_of(" ");
            if(iPos!=string::npos)
                sNoteVal=sVal.substr(iPos,string::npos);

            sNote+=sNoteVal+"\n";
        }

        if(sKey=="DISZ")
        {
            stringstream ss(sVal); ss >> dTick;
        }

        if(sKey=="GCAT")
        {
            stringstream ss(sVal); ss >> sGlassCatalog; //TODO use
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

        if(sKey=="ENPD") //aperture diameter
        {
            stringstream ss(sVal); ss >> dApertureDiameter;
        }

        if(sKey=="GLAS")
        {
            string sFirstWord;
            auto iPos=sVal.find_first_of(" ");
            if(iPos!=string::npos)
                sFirstWord=sVal.substr(0,iPos);

            if(sFirstWord=="MIRROR")
                sType="reflect";
            else
                sType=sFirstWord; //TODO more robust test
        }
    }

    if(!sNote.empty())
        pOD->set_note(sNote);

    // set the last surface type as image, TODO better check
    if(pOD->nb_surface()!=0)
    {    pOD->set_type(pOD->nb_surface()-1,"image");

        if(dApertureDiameter>=0.)
            pOD->set(0,DIAMETER,dApertureDiameter*dDimensionFactor);
    }

    // TODO add field of view

    return pOD;
}
////////////////////////////////////////////////////////////////////////////////
