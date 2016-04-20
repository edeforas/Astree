// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _OpticalDevice_
#define _OpticalDevice_

#include <string>
#include <vector>
using namespace std;

#define RADIUS_CURVATURE_INFINITY 9e99
#define FD_VALID_MAX 1e10
#define SPOT_SIZE_INFINITY 9e99

enum eSurfaceparameter
{
    R4,
    R6,
    R8,
    R10

};

#include "ImageQuality.h"

#include "Surface.h"

class Light;

class OpticalDevice
{
public:   
    OpticalDevice();
    OpticalDevice(const OpticalDevice &rDevice);
    OpticalDevice& operator=(const OpticalDevice& rDevice);
    virtual ~OpticalDevice();

    // surface management
    void insert_surface(int iPos);
    void delete_surface(int iPos);
    int nb_surface() const;

    // surface type management
    void set_type(int iSurf,string sType);
    string type(int iSurf) const;

    // note management
    void set_note(string sNote);
    string note() const;

    // surface geometry settings
    void set_conic(int iSurf,double dConic);
    double conic(int iSurf);
    bool is_aspheric() const;
    void set_radius_curvature(int iSurf,double dRadiusCurvature);
    double radius_curvature(int iSurf) const;
    void set_poly_aspheric(int iSurf,double dR4,double dR6,double dR8,double dR10);
    bool compute_surface_profile(int iSurface,double dX,double dY,double& dZ);

    // surface data accessor
    void set(int iSurface,eSurfaceparameter eParam,double dParam);
    double get(int iSurface,eSurfaceparameter eParam) const;

    //diameter settings
    void set_diameter(int iSurf,double dDiameter);
    void set_auto_diameter(int iSurf,bool bAutoDiameter);
    bool get_auto_diameter(int iSurf);
    double diameter(int iSurf);
    void set_inner_diameter(int iSurf,double dInnerDiameter);
    void set_auto_inner_diameter(int iSurf,bool bAutoInnerDiameter);
    bool get_auto_inner_diameter(int iSurf) const;
    double inner_diameter(int iSurf);
    bool has_inner_diameter() const;

    // z settings
    void set_convention(string sConvention);
    string convention() const;
    void set_z(int iSurface, double dVal);
    double z(int iSurface);
    double global_z(int iSurface);
    void set_autofocus(int iSurface,bool bAutofocus);
    bool get_autofocus(int iSurface);

    // comment settings
    void set_comment(int iSurface,string sComment);
    string comment(int iSurface) const;
    bool has_comment() const;

    // light properties
    void set_half_field_of_view(double dHalfFov);
    double half_field_of_view() const;
    int nb_intermediate_angles() const;
    void set_nb_intermediate_angles(int iNbAngles);
    void set_light_colors(const string & _sLightColors);
    string light_colors() const;
    void set_light_grid(int iGridX,int iGridY);

    // one light computation
    void compute_light(Light* pLight,int iSurface, double dTilt, int iGridX, int iGridY);
    const ImageQuality *get_image_quality();

private:
    void initialise_light(Light *pLight, double dTilt, int iGridX, int iGridY);
    void ray_trace();
    void update_absolute_surfaces();
    void update_relative_surfaces();

    vector<double> _vdRelativeTick;
    vector<Surface> _theSurf;

    string _sConvention;
    bool _bIsRelativeSurface;

    string _sNote;

    double _dHalfFov;
    int _iGridX,_iGridY;
    int _iNbAngles;
    string _sLightColors;
    bool _bMustRetrace;
    ImageQuality _imageQuality;
};

#endif
