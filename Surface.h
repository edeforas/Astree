// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef Surface_
#define Surface_

#include <string>
using namespace std;

#define MAX_DIAMETER 1.e10
#define LARGER_RADIUS_CHECK_TOLERANCIS 1e-6 //1 nm
#define CURVATURE_FLAT 1e-20

class Light;
class Glass;
class Photon;

class Surface
{
public:
    Surface();
    Surface(const Surface &rSurf);
    Surface& operator=(const Surface& rSurf);
    virtual ~Surface();

    string name();
    void name(string sName);

    string type() const;
    bool set_type(string sType);

    void set_x(double dx);
    double x() const;

    void set_y(double dy);
    double y() const;

    void set_z(double dz);
    double z() const;

    void set_diameter(double dDiameter);
    void set_auto_diameter(bool bAutoDiameter);
    bool get_auto_diameter() const;
    double diameter() const;

    void set_inner_diameter(double dInnerDiameter);
    void set_auto_inner_diameter(bool bAutoInnerDiameter);
    bool get_auto_inner_diameter() const;
    double inner_diameter() const;

    void set_radius_curvature(double dRadiusCurvature);
    double radius_curvature() const;

    void set_curvature(double dCurvature);
    double curvature() const;

    void set_R4(double dR4);
    void set_R6(double dR6);
    void set_R8(double dR8);
    void set_R10(double dR10);

    double R4() const;
    double R6() const;
    double R8() const;
    double R10() const;
    bool is_aspheric() const;

    void set_conic(double dConic);
    double conic() const;

    void receive(Light& l);

    bool compute_z(double x,double y,double& z);

    void set_comment(string sComment);
    string comment() const;

private:
    bool verify_in_surface(double dX,double dY);
    void local_ref(Photon& p) const;
    void global_ref(Photon& p) const;

    void stop(Light& l);
    void reflect(Light& l);
    void transmit(Light& l);

    void stop_photon(Photon& p);
    void reflect_photon(Photon& p);
    void transmit_photon(Photon& p);

    bool compute_normal(double x,double y, double z,double& nx,double& ny,double &nz);
    void update_geometry();

    Glass* _pMaterial;
    Glass* _pMaterialPrev;
    Glass* _pMaterialNext;

    double _dConic;
    double _dCurvature;

    bool _bIsFlat;
    bool _bIsSpherical;
    bool _bIsConic;
    bool _bIsAspheric;
    bool _bIsPerfect; //simple geometrical optic
    double _dR4, _dR6, _dR8, _dR10; // _dR4 = coef of h**4 , etc ( with h**2=x*x+y*y)

    string _sType;
    string _sComment;

    double _x,_y,_z;
    double _dDiameter,_dDiameter2;
    double _dInnerDiameter,_dInnerDiameter2;

    bool _bAutoDiameter;
    bool _bAutoInnerDiameter;
};

#endif
