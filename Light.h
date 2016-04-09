// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _Light_
#define _Light_

#include <string>
#include <vector>
using namespace std;

class Photon;
class Material;

class Light
{
public:
    Light(/*int iDimX=35,int iDimY=35*/);
    virtual ~Light();
    vector<Photon> get_all_photons();
    void set_nb_photons(int iNbX,int iNbY);

    Photon& get_photon(int iPos);
    int nb_photon();

    void set_tilt(double dTiltX,double dTiltY);
    void get_tilt(double& dTiltX,double& dTiltY) const;
    Material& material();
    void set_material(Material* pM);

    void set_colors(const string & vsAllColors);
    static void calc_colors(string sColors,vector<double>& vdLambda,vector<int>& vdVisualColors);
    int get_visual_color(double dLambda) const;

    void set_geometry(double dZ,double dDiameter);

    void compute_spot_size();

    void get_spot_center(double& dCenterX,double& dCenterY) const;
    double spot_size() const;
    double get_FD() const;

    double airy_radius() const;
    double spot_vs_airy() const;

    void init_vignetting();
    double vignetting() const;

private:
    void init();

    vector<Photon> _vPhotons;
    Material* _pFirstMaterial;
    Material* _pMaterial;
    bool _bMustInit;
    double _dTiltX,_dTiltY;
    int _iDimX,_iDimY,_iNbPhotons;
    int _iNbPhotonsInitVignetting;
    double _dDiameter,_dZ;
    double _dCenterX,_dCenterY,_dSpotSize;
    double _dFD;
    double _dVignetting;

    string _sAllColors;
    bool _bYellowBlack;
};

#endif

