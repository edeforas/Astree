// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <cassert>
#include <cmath>
#include <algorithm>
using namespace std;

#include "Light.h"
#include "Photon.h"
#include "GlassManager.h"

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
#define PURPLE 0x772176

#define LAMBDA_IR 1000.e-3
#define LAMBDA_RED 656.2725e-3
#define LAMBDA_YELLOW 587.5618e-3
#define LAMBDA_GREEN 546.0740e-3
#define LAMBDA_BLUE 486.1327e-3
#define LAMBDA_UV 300.e-3

//////////////////////////////////////////////////////////////////////////////
Light::Light()
{
    _iNbPhotons=0;
    _dTiltX=0.;
    _dTiltY=0.;
    _dSpotSize=0.;

    _dCenterX=0.;
    _dCenterY=0.;

    _dDiameter=0.;
    _dZ=0.;
    _dFD=1.e99;

    _bMustInit=true;
    _bYellowBlack=false;

    _pFirstMaterial=0;
    _pMaterial=_pFirstMaterial;

    _dVignetting=-1.;
    _iNbPhotonsInitVignetting=0;
    _bIsInfinite=false;
    _bIsValid=false;

    _iDimX=0;
    _iDimY=0;
}
//////////////////////////////////////////////////////////////////////////////
Light::~Light()
{
    GlassManager::singleton().destroy(_pFirstMaterial);
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
    init();

    assert(iPos>=0);
    assert(iPos<_iNbPhotons);

    return _vPhotons[iPos];
}
//////////////////////////////////////////////////////////////////////////////
vector<Photon> Light::get_all_photons()
{
    init();
    return _vPhotons;
}
//////////////////////////////////////////////////////////////////////////////
const vector<Photon>& Light::photons() const
{
    return _vPhotons; //warning _vPhotons may uninitialized TODO
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
    //TODO remove init or set public or use lazy + acccessor

    if (_bMustInit==false)
        return;

    //light direction
    double dTanX=-tan(_dTiltX);
    double dTanY=-tan(_dTiltY);
    double dIdentN=+1.;

    //TODO normalize dItentN?

    vector<double> vdLambda;
    vector<int> viVisualColors;
    calc_colors(_sAllColors, vdLambda, viVisualColors);

    _iNbPhotons=(int)(_iDimX*_iDimY*vdLambda.size());
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

                // can be optimised if needed
                double dPosX=(i-iNMx)*_dDiameter/(double)iNMx/2.;

                p.x=dPosX-dTranslateX;
                p.y=dPosY-dTranslateY;
                p.z=_dZ-dRadius;
                p.dx=dTanX;
                p.dy=dTanY;
                p.dz=dIdentN;

                p.valid=true;
                assert(p.is_valid());
                iPhoton++;
            }
        }
    }

    GlassManager::singleton().destroy(_pFirstMaterial);
    _pFirstMaterial=GlassManager::singleton().create("Air");
    _pMaterial=_pFirstMaterial;
    _bIsInfinite=false;
    _bIsValid=true;

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
        vdVisualColors.push_back(PURPLE);
    }
}
//////////////////////////////////////////////////////////////////////////////
int Light::get_visual_color(double dLambda) const
{
    if (dLambda==LAMBDA_IR)
        return BROWN;

    if (dLambda==LAMBDA_RED)
        return RED;

    if (dLambda==LAMBDA_YELLOW)
    {
        if(_bYellowBlack)
            return DARK_GREY;
        else
            return YELLOW;
    }

    if (dLambda==LAMBDA_GREEN)
        return GREEN;

    if (dLambda==LAMBDA_BLUE)
        return BLUE;

    if (dLambda==LAMBDA_UV)
        return PURPLE;

    return 0;
}
//////////////////////////////////////////////////////////////////////////////
void Light::set_colors(const string & sAllColors)
{
    _sAllColors=sAllColors;

    if (_sAllColors.empty())
        _sAllColors="YellowBlack.";

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
Glass& Light::material()
{
    init();
    assert(_pMaterial);
    return *_pMaterial;
}
//////////////////////////////////////////////////////////////////////////////
void Light::set_material(Glass* pM)
{
    assert(pM!=0);
    _pMaterial=pM;
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
void Light::set_image_infinite(bool bInfinite)
{
    (void)bInfinite;
     //_bIsInfinite=bInfinite;
}
//////////////////////////////////////////////////////////////////////////////
bool Light::is_image_infinite() const
{
    return _bIsInfinite;
}
//////////////////////////////////////////////////////////////////////////////
void Light::compute_spot_size(bool bInfinite) //todo remove bInfinite flag here
{
    double dXS=0., dYS=0.;
    double dMinX=0.,dMaxX=0.,dMinY=0.,dMaxY=0.;
    double dMinXR=0.,dMaxXR=0.,dMinYR=0.,dMaxYR=0.; //rotated 45deg

    double px,py;//,pz;

    double dDxCentral=0.;
    double dDyCentral=0.;
    double dDzCentral=0.;

    _bIsInfinite=bInfinite;

    //compute bounding octogon (8 edges)
    int iNbValidPhoton=0;
    for (int i=0;i<_iNbPhotons;i++)
    {
        const Photon& p=_vPhotons[i];

        if (p.is_valid()==false)
            continue;

        if(bInfinite)
        {
            //compute the x and y angle in rad, store in px, py
            px=atan2(p.dx,p.dz);
            py=atan2(p.dy,p.dz);
        }
        else
        {
            //use intersection at focal plane
            px=p.x; py=p.y;
        }

        if (iNbValidPhoton==0) //first time tinit
        {
            dMinX=px;
            dMaxX=px;
            dMinY=py;
            dMaxY=py;

            dMinXR=px-py;
            dMaxXR=dMinXR;
            dMinYR=px+py;
            dMaxYR=dMinYR;
        }
        else
        {
            // bounding rectangle
            dMinX=std::min(dMinX,px);
            dMaxX=std::max(dMaxX,px);
            dMinY=std::min(dMinY,py);
            dMaxY=std::max(dMaxY,py);

            // bounding rotated(45deg) rectangle
            dMinXR=std::min(dMinXR,px-py);
            dMaxXR=std::max(dMaxXR,px-py);
            dMinYR=std::min(dMinYR,px+py);
            dMaxYR=std::max(dMaxYR,px+py);
        }

        dXS+=px;
        dYS+=py;

        double pdx=p.dx,pdy=p.dy, pdz=p.dz;
        Vector3D::normalize(pdx,pdy,pdz); //TODO remove using mean axis(max,min)
        dDxCentral+=pdx;
        dDyCentral+=pdy;
        dDzCentral+=pdz;

        iNbValidPhoton++;
    }

    if(iNbValidPhoton==0)
    {
        _bIsValid=false;
        _dCenterX=0.;
        _dCenterY=0.;
        _dSpotSize=1.e99;
        _dFD=1.e99;
        return;
    }

    if(bInfinite)
        _dFD=1.e99;
    else
    {
        // compute F/D

        Vector3D::normalize(dDxCentral,dDyCentral,dDzCentral);

        //compute cos on mean axis
        double dMinCos=3.;
        for (int i=0;i<_iNbPhotons;i++)
        {
            const Photon& p=_vPhotons[i];

            if (p.is_valid()==false)
                continue;

            // project p on axis
            double dCosP=p.dx*dDxCentral+p.dy*dDyCentral+p.dz*dDzCentral;
            if(dCosP<0)
            {
                _bIsValid=false;
                _dCenterX=0.;
                _dCenterY=0.;
                _dSpotSize=1.e99;
                _dFD=1.e99;
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

        if(_dFD<0.01) //TODO
        {
            _bIsValid=false;
            _dCenterX=0.;
            _dCenterY=0.;
            _dSpotSize=1.e99;
            _dFD=1.e99;
            return;
        }
    }

    _dCenterX=dXS/iNbValidPhoton;
    _dCenterY=dYS/iNbValidPhoton;

    _dSpotSize=std::max(dMaxX-dMinX,dMaxY-dMinY);
    double dSpotSizeRotated=std::max(dMaxXR-dMinXR,dMaxYR-dMinYR)*0.7071067811865475; //sqrt(2)/2
    _dSpotSize=std::max(_dSpotSize,dSpotSizeRotated);

    if(bInfinite)
    {
        //convert to deg
        _dCenterX*=180./PI;
        _dCenterY*=180./PI;
        _dSpotSize*=180./PI;
    }

    _dVignetting=100.*(double)iNbValidPhoton/_iNbPhotonsInitVignetting;
    _bIsValid=true;
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
    if(is_image_infinite())
        return 0.02; // in deg ; for now ;  https://en.wikipedia.org/wiki/Naked_eye
    else
        return 1.22*get_FD()*(LAMBDA_YELLOW)*1.e-3; // use yellow light
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
bool Light::is_valid() const
{
    return _bIsValid;
}
//////////////////////////////////////////////////////////////////////////////

