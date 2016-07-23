// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <sstream>
#include <fstream>
using namespace std;

#include "GlassCatalogIO.h"
#include "GlassSellmeier.h"
#include "GlassManager.h"

////////////////////////////////////////////////////////////////////////////////
bool GlassCatalogIO::load(string sFile,GlassManager& pManager)
{
    ifstream f(sFile.c_str(),ios::in);

    bool bGlassPending=false;

    string sName;
    string sMaker;
    double B1;
    double B2;
    double B3;
    double C1;
    double C2;
    double C3;

    auto iIndexStartMaker=sFile.find_last_of('\\');
    if( (iIndexStartMaker!=string::npos) )
        sMaker=sFile.substr(iIndexStartMaker+1);
    auto iIndexSeparator=sMaker.find_first_of('_');
    if( (iIndexSeparator!=string::npos) )
        sMaker=sMaker.substr(0,iIndexSeparator);

    //TODO

    while(!f.eof())
    {
        string sLine;
        ::getline(f,sLine);

        //trim first space if any
        unsigned int iPos=sLine.find_first_not_of(" ");
        if(iPos!=string::npos)
            sLine=sLine.substr(iPos);

        if(sLine.size()<2)
            continue; // nothing to parse //TODO test bGlassPending before exit

        string sKey=sLine.substr(0,2);
        string sVal=sLine.substr(2); // can be empty

        //trim first space if any
        unsigned int iPos2=sVal.find_first_not_of(" ");
        if(iPos2!=string::npos)
            sVal=sVal.substr(iPos2);

        if(sKey=="CC")
        {
            //			pCatalog->set_comment(sVal);
        }

        if(sKey=="NM")
        {
            if(bGlassPending)
            {
                //create and store the glass
                GlassSellmeier* pGlass=new GlassSellmeier;
                pGlass->set_maker(sMaker);
                pGlass->set_name(sName);               
                pGlass->set_coefs(B1,B2,B3,C1,C2,C3);
                pGlass->set_solid_color(8844025); //todo
                pManager.inject(pGlass);
            }

            //compute subkey
            stringstream ss(sVal);
            ss >> sName;

            bGlassPending=true;
        }

        if(sKey=="CD") //Selmeier formula
        {
            stringstream ss(sVal);
            ss >> B1 >> C1 >> B2 >> C2 >> B3 >> C3 ;
        }

        //	pCatalog->set_maker("unknown"); //todo
    }

    return true;
}
////////////////////////////////////////////////////////////////////////////////
