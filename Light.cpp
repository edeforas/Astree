// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <cassert>
#include <cmath>
#include <algorithm>
using namespace std;

#include "Light.h"
#include "Photon.h"
#include "MaterialManager.h"

#include "Vector3D.h"

#define PI (3.1415926535897)
#define RAD_TO_DEG (360./2./PI)
#define DEG_TO_RAD (PI*2./360.)

#define RGB(r,g,b) ((r << 16) + (g <<8) + b)

#define RED RGB(255,0,0)
#define GREEN RGB(0,255,0)
#define BLUE RGB(0,0,255)
#define DARK_GREY RGB(32,32,32)
#define YELLOW 0xede500
#define BROWN 0x74572e
#define VIOLET 0x772176

#define LAMBDA_IR 1000.e-3
#define LAMBDA_RED 656.2725e-3
#define LAMBDA_YELLOW 587.5618e-3
#define LAMBDA_GREEN 546.0740e-3
#define LAMBDA_BLUE 486.1327e-3
#define LAMBDA_UV 300.e-3

//////////////////////////////////////////////////////////////////////////////
Light::Light(/*int iDimX,int iDimY*/)
{
   // set_nb_photons(0,0);
    _iNbPhotons=0;
    _dTiltX=0.;
    _dTiltY=0.;
    _dSpotSize=0.;

    _dCenterX=0.;
    _dCenterY=0.;

    _dDiameter=0.;
    _dZ=0.;
    _dFD=0.;

    _bMustInit=true;
    _bYellowBlack=false;

    _pFirstMaterial=0;
    _pMaterial=_pFirstMaterial;

    _dVignetting=-1.;
    _iNbPhotonsInitVignetting=0;

    _iDimX=0;
    _iDimY=0;
}
//////////////////////////////////////////////////////////////////////////////
Light::~Light()
{
    MaterialManager::singleton().destroy(_pFirstMaterial);
}
//////////////////////////////////////////////////////////////////////////////
int Light::nb_photon()
{
    init();
    return _iNbPhotons;
}
//////////////////////////////////////////////////////////////////////////////
Photon& Light::get_photon(int iPos)
{
    assert(iPos>=0);
    assert(iPos<_iNbPhotons);

    init();
    return _vPhotons[iPos];
}
//////////////////////////////////////////////////////////////////////////////
vector<Photon> Light::get_all_photons()
{
    init();
    return _vPhotons;
}
//////////////////////////////////////////////////////////////////////////////
void Light::set_geometry(double dZ,double dDiameter)
{
    assert(dDiameter!=-1.);
    _dZ=dZ;
    _dDiameter=dDiameter;
    _bMustInit=true;
}
//////////////////////////////////////////////////////////////////////////////
void Light::init()
{
    if (_bMustInit==false)
        return;

    //direction de la lumiere
    double dTanX=-tan(_dTiltX);
    double dTanY=-tan(_dTiltY);
    double dIdentN=+1.;
    //TODO normaliser dItentN?

    vector<double> vdLambda;
    vector<int> viVisualColors;
    calc_colors(_sAllColors, vdLambda, viVisualColors);

    _iNbPhotons=_iDimX*_iDimY*vdLambda.size();
    _vPhotons.resize(_iNbPhotons);

    int iPhoton=0;
    double dRadius=_dDiameter; //TODO
    double dTranslateX=dTanX*dRadius;
    double dTranslateY=dTanY*dRadius;

    for (unsigned int iLambda=0;iLambda<vdLambda.size();iLambda++)
    {
        double dLambda=vdLambda[iLambda];
        //int iVisualColor=viVisualColors[iLambda];
        int iNMx=_iDimX/2;
        int iNMy=_iDimY/2;
        // assert(_iDimX is odd);
        // assert(_iDimY is odd);

        for (int j=0; j<_iDimY;j++)
        {
            double dPosY=(j-iNMy)*_dDiameter/(double)iNMy/2.;
            if (_iDimY==1)
                dPosY=0;

            for (int i=0;i<_iDimX;i++)
            {
                Photon& p=_vPhotons[iPhoton];
                p.set_lambda(dLambda);

                // a optimiser au besoin
                double dPosX=(i-iNMx)*_dDiameter/(double)iNMx/2.;

                p.x=dPosX-dTranslateX;
                p.y=dPosY-dTranslateY;
                p.z=_dZ-dRadius; //recul
                p.dx=dTanX;
                p.dy=dTanY;
                p.dz=dIdentN;

                p.valid=true;
                assert(p.is_valid());
                iPhoton++;
            }
        }
    }

    MaterialManager::singleton().destroy(_pFirstMaterial);
    _pFirstMaterial=MaterialManager::singleton().create("Air");
    _pMaterial=_pFirstMaterial;
    _bMustInit=false;
}
//////////////////////////////////////////////////////////////////////////////
void Light::set_tilt(double dTiltX,double dTiltY)
{
    _dTiltX=dTiltX*DEG_TO_RAD;
    _dTiltY=dTiltY*DEG_TO_RAD;
    _bMustInit=true;
}
//////////////////////////////////////////////////////////////////////////////
void Light::get_tilt(double& dTiltX,double& dTiltY) const
{
    dTiltX=_dTiltX*RAD_TO_DEG;
    dTiltY=_dTiltY*RAD_TO_DEG;
}
//////////////////////////////////////////////////////////////////////////////
void Light::calc_colors(string sColors,vector<double>& vdLambda,vector<int>& vdVisualColors)
{
    vdLambda.clear();
    vdVisualColors.clear();

    if (sColors.find("IR.")!=string::npos)
    {
        vdLambda.push_back(LAMBDA_IR);
        vdVisualColors.push_back(BROWN); //brown
    }

    if (sColors.find("Red.")!=string::npos)
    {
        vdLambda.push_back(LAMBDA_RED);
        vdVisualColors.push_back(RED);
    }

    if (sColors.find("YellowBlack.")!=string::npos)
    {
        vdLambda.push_back(LAMBDA_YELLOW);
        vdVisualColors.push_back(DARK_GREY);
    }

    if (sColors.find("Yellow.")!=string::npos)
    {
        vdLambda.push_back(LAMBDA_YELLOW);
        vdVisualColors.push_back(YELLOW); //yellow
    }

    if (sColors.find("Green.")!=string::npos)
    {
        vdLambda.push_back(LAMBDA_GREEN);
        vdVisualColors.push_back(GREEN);
    }

    if (sColors.find("Blue.")!=string::npos)
    {
        vdLambda.push_back(LAMBDA_BLUE);
        vdVisualColors.push_back(BLUE);
    }

    if (sColors.find("UV.")!=string::npos)
    {
        vdLambda.push_back(LAMBDA_UV);
        vdVisualColors.push_back(VIOLET); //violet
    }
}
//////////////////////////////////////////////////////////////////////////////
int Light::get_visual_color(double dLambda) const
{
    if (dLambda==LAMBDA_IR) //IR
        return BROWN; //brown

    if (dLambda==LAMBDA_RED) //Red
        return RED; //red

    if (dLambda==LAMBDA_YELLOW) //Yellow
    {
        if(_bYellowBlack)
            return DARK_GREY; //dark grey
        else
            return YELLOW; //Yellow
    }

    if (dLambda==LAMBDA_GREEN) //Green
        return GREEN; //Green

    if (dLambda==LAMBDA_BLUE) //Blue
        return BLUE; //Blue

    if (dLambda==LAMBDA_UV) //UV
        return VIOLET; //violet

    return 0;
}
//////////////////////////////////////////////////////////////////////////////
void Light::set_colors(const string & sAllColors)
{
    _sAllColors=sAllColors;

    if (_sAllColors.empty())
    {
        _sAllColors="YellowBlack.";
    }

    // _bYellowBlack=(find(_sAllColors.begin(),_sAllColors.end(),"YellowBlack")!=_sAllColors.end()); TODO

    _bYellowBlack=(_sAllColors=="YellowBlack.");

    _bMustInit=true;
}
//////////////////////////////////////////////////////////////////////////////
void Light::set_nb_photons(int iNbX,int iNbY)
{
    assert(iNbX>0);
    assert(iNbY>0);

    _iDimX=iNbX;
    _iDimY=iNbY;

    _bMustInit=true;
}
//////////////////////////////////////////////////////////////////////////////
Material& Light::material()
{
    init();
    assert(_pMaterial);
    return *_pMaterial;
}
//////////////////////////////////////////////////////////////////////////////
void Light::set_material(Material* pM)
{
    assert(pM!=0);
    _pMaterial=pM;
    //  _bPhotonsReady=false;
}
//////////////////////////////////////////////////////////////////////////////
void Light::get_spot_center(double& dCenterX,double& dCenterY) const
{
    dCenterX=_dCenterX;
    dCenterY=_dCenterY;
}
//////////////////////////////////////////////////////////////////////////////
double Light::spot_size() const
{
    return _dSpotSize;
}
//////////////////////////////////////////////////////////////////////////////
void Light::compute_spot_size()
{
    double dXS=0., dYS=0.;
    double dMinX=0.,dMaxX=0.,dMinY=0.,dMaxY=0.;

    double dDxCentral=0.;
    double dDyCentral=0.;
    double dDzCentral=0.;

    int iNbValidPhoton=0;
    for (int i=0;i<_iNbPhotons;i++)
    {
        Photon& p=_vPhotons[i];

        if (p.is_valid()==false)
            continue;

        if (iNbValidPhoton==0)
        {
            dMinX=p.x;
            dMaxX=p.x;
            dMinY=p.y;
            dMaxY=p.y;
        }
        else
        {
            dMinX=std::min(dMinX,p.x);
            dMaxX=std::max(dMaxX,p.x);
            dMinY=std::min(dMinY,p.y);
            dMaxY=std::max(dMaxY,p.y);
        }

        dXS+=p.x;
        dYS+=p.y;

        Vector3D::normalize(p.dx,p.dy,p.dz);
        dDxCentral+=p.dx;
        dDyCentral+=p.dy;
        dDzCentral+=p.dz;

        iNbValidPhoton++;
    }

    if(iNbValidPhoton==0)
    {
        _dCenterX=0.;
        _dCenterY=0.;
        _dSpotSize=0.;
        _dFD=0.;
        return;
    }

    Vector3D::normalize(dDxCentral,dDyCentral,dDzCentral);

    //calcule le cos l'axe moyen
    double dMinCos=3.;
    for (int i=0;i<_iNbPhotons;i++)
    {
        Photon& p=_vPhotons[i];

        if (p.is_valid()==false)
            continue;

        //calcule le projette de p sur l'axe
        double dCosP=p.dx*dDxCentral+p.dy*dDyCentral+p.dz*dDzCentral;
        if(dCosP<0)
        {
            _dCenterX=0.;
            _dCenterY=0.;
            _dSpotSize=1.e99; //todo a cleaner error message
            _dFD=0.;
            return;
        }

        assert(dCosP<1.0001);
        assert(dCosP>-0.0001);

        if(dMinCos>dCosP)
            dMinCos=dCosP;
    }

    assert(dMinCos>-0.0001);
    assert(dMinCos<1.0001);

    if(dMinCos>1.)
        dMinCos=1.;

    _dFD=0.5/tan(acos(dMinCos)); //TODO

    _dCenterX=dXS/iNbValidPhoton;
    _dCenterY=dYS/iNbValidPhoton;

    _dSpotSize=std::max(dMaxX-dMinX,dMaxY-dMinY);

    _dVignetting=100.*(double)iNbValidPhoton/_iNbPhotonsInitVignetting;
}
//////////////////////////////////////////////////////////////////////////////
double Light::get_FD() const
{
    return _dFD;
}
//////////////////////////////////////////////////////////////////////////////
double Light::spot_vs_airy() const
{
    return spot_size()/2./airy_radius();
}
//////////////////////////////////////////////////////////////////////////////
double Light::airy_radius() const
{
    return 1.22*get_FD()*(LAMBDA_YELLOW/*560.e-3*/)*1.e-3; //lumiere jaune en microns //TODO use defines
}
//////////////////////////////////////////////////////////////////////////////
double Light::vignetting() const
{
    return _dVignetting;
}
//////////////////////////////////////////////////////////////////////////////
void Light::init_vignetting()
{
    _iNbPhotonsInitVignetting=0;

    for (int i=0;i<_iNbPhotons;i++)
        if (_vPhotons[i].is_valid())
            _iNbPhotonsInitVignetting++;
}
//////////////////////////////////////////////////////////////////////////////
