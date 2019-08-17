// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <sstream>
#include <fstream>
using namespace std;

#include "GlassCatalogIo.h"
#include "GlassSellmeier.h"
#include "GlassExtended2.h"
#include "GlassManager.h"

////////////////////////////////////////////////////////////////////////////////
bool GlassCatalogIO::load(string sFile,GlassManager& pManager)
{
    ifstream f(sFile.c_str(),ios::in);

    bool bGlassPending=false;

    string sName;
    int iGlassFormula = 0;
    string sMaker;
    double B1=0.;
    double B2=0.;
    double B3=0.;
    double C1=0.;
    double C2=0.;
    double C3=0.;
    double B4=0.;
    double B5=0.;

    auto iIndexStartMaker=sFile.find_last_of('\\');
    if( (iIndexStartMaker!=string::npos) )
        sMaker=sFile.substr(iIndexStartMaker+1);
    auto iIndexSeparator=sMaker.find_first_of("_. -");
    if( (iIndexSeparator!=string::npos) )
        sMaker=sMaker.substr(0,iIndexSeparator);

    while(!f.eof())
    {
        string sLine;
        std::getline(f,sLine);

        //trim first space if any
        size_t iPos=sLine.find_first_not_of(" ");
        if(iPos!=string::npos)
            sLine=sLine.substr(iPos);

        if(sLine.size()<2)
            continue; // nothing to parse //TODO test bGlassPending before exit

        string sKey=sLine.substr(0,2);
        string sVal=sLine.substr(2); // can be empty

        //trim first space if any
        size_t iPos2=sVal.find_first_not_of(" ");
        if(iPos2!=string::npos)
            sVal=sVal.substr(iPos2);

        if(sKey=="CC")
        {
            //	pCatalog->set_comment(sVal);
        }

        if(sKey=="NM")
        {
            if(bGlassPending)
            {
                if(iGlassFormula==2) //Sellmeier
                {
                    //create and store the glass
                    GlassSellmeier* pGlass=new GlassSellmeier;
                    pGlass->set_maker(sMaker);
                    pGlass->set_name(sName);
                    pGlass->set_coefs(B1,B2,B3,C1,C2,C3);
                    pGlass->set_solid_color(8844025); //TODO
                    pManager.inject(pGlass);
                }
                else if(iGlassFormula==12) //Extended2
                {
                    //create and store the glass
                    GlassExtended2* pGlass=new GlassExtended2;
                    pGlass->set_maker(sMaker);
                    pGlass->set_name(sName);
                    pGlass->set_coefs(B1,C1,B2,C2,B3,C3,B4,B5);
                    pGlass->set_solid_color(8844025); //TODO
                    pManager.inject(pGlass);
                }
                else
                {
                    //TODO
                }
            }

            //compute subkey
            stringstream ss(sVal);
            ss >> sName >> iGlassFormula;

            bGlassPending=true;
        }

        if(sKey=="CD") //GlassCoef formula
        {
            stringstream ss(sVal);
            ss >> B1 >> C1 >> B2 >> C2 >> B3 >> C3 >> B4 >> B5; //may have more coef than needed
        }

        //	pCatalog->set_maker("unknown"); //TODO
    }

    return true;
}
////////////////////////////////////////////////////////////////////////////////
