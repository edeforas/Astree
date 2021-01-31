# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef Surface_
#define Surface_

#include <string>
using namespace std

#define MAX_DIAMETER 1.e10
#define LARGER_RADIUS_CHECK_TOLERANCIS 1e-6 #1 nm
#define CURVATURE_FLAT 1e-20

class Light
class Glass
class Photon

class Surface
public:
    Surface()
    Surface( Surface &rSurf)
    Surface& operator=( Surface& rSurf)
    virtual ~Surface()

    string type()
    bool set_type(string sType)

    void set_x(double dx)
    double x()

    void set_y(double dy)
    double y()

    void set_z(double dz)
    double z()

    void set_diameter(double dDiameter)
    void set_auto_diameter(bool bAutoDiameter)
    bool get_auto_diameter()
    double diameter()

    void set_inner_diameter(double dInnerDiameter)
    void set_auto_inner_diameter(bool bAutoInnerDiameter)
    bool get_auto_inner_diameter()
    double inner_diameter()

    void set_radius_curvature(double dRadiusCurvature)
    double radius_curvature()

    void set_curvature(double dCurvature)
    double curvature()

    void set_R4(double dR4)
    void set_R6(double dR6)
    void set_R8(double dR8)
    void set_R10(double dR10)

    double R4()
    double R6()
    double R8()
    double R10()
    bool is_aspheric()

    void set_conic(double dConic)
    double conic()

    void receive(Light& l)

    bool compute_z(double x, y, z)

    void set_comment( string& sComment)
    string comment()

private:
    bool update_auto_diameter(double dX, dY); #and return True if point is inside diameters
    void local_ref(Photon& p)
    void global_ref(Photon& p)

    void stop(Light& l)
    void reflect(Light& l)
    void transmit(Light& l)

    void stop_photon(Photon& p)
    void reflect_photon(Photon& p)
    void transmit_photon(Photon& p)

    bool compute_normal(double x, y, z, nx, ny, &nz)
    void update_geometry()

    Glass* _pMaterial
    Glass* _pMaterialPrev
    Glass* _pMaterialNext

    double _dConic
    double _dCurvature

    bool _bIsFlat
    bool _bIsSpherical
    bool _bIsConic
    bool _bIsAspheric
    bool _bIsPerfect; #simple geometrical optic
    double _dR4, _dR6, _dR8, _dR10; # _dR4 = coef of h**4 , etc ( with h**2=x*x+y*y)

    string _sType
    string _sComment

    double _x,_y,_z
    double _dDiameter,_dDiameter2
    double _dInnerDiameter,_dInnerDiameter2

    bool _bAutoDiameter
    bool _bAutoInnerDiameter


#endif
