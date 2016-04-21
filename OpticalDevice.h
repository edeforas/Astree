// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _OpticalDevice_
#define _OpticalDevice_

#include <string>
#include <vector>
using namespace std;

#include "ImageQuality.h"
#include "Surface.h"
class Light;

#define RADIUS_CURVATURE_INFINITY 9e99
#define FD_VALID_MAX 1e10
#define SPOT_SIZE_INFINITY 9e99

enum eSurfaceparameter //to be used with get/set functions
{
    DIAMETER,
    AUTO_DIAMETER, // set with 0 or 1
    INNER_DIAMETER,
    AUTO_INNER_DIAMETER, // set with 0 or 1
    CONIC,
    RADIUS_CURVATURE,
    R4,
    R6,
    R8,
    R10
};


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

    // surface data accessor
    void set(int iSurface,eSurfaceparameter eParam,double dParam);
    double get(int iSurface,eSurfaceparameter eParam); //not const due to lazy computation

    // global properties getter
    bool has_aspheric() const;
    bool has_inner_diameter() const;

    // z settings
    void set_convention(string sConvention);
    string convention() const;
    void set_z(int iSurface, double dVal);
    double z(int iSurface);
    double global_z(int iSurface);
    void set_autofocus(int iSurface,bool bAutofocus);
    bool get_autofocus(int iSurface);
    bool compute_surface_profile(int iSurface,double dX,double dY,double& dZ);

    // comment settings
    void set_comment(int iSurface,string sComment);
    string comment(int iSurface) const;
    bool has_comment() const;

    // input light properties
    void set_half_field_of_view(double dHalfFov);
    double half_field_of_view() const;
    int nb_intermediate_angles() const;
    void set_nb_intermediate_angles(int iNbAngles);
    void set_light_colors(const string & _sLightColors);
    string light_colors() const;
    void set_light_grid(int iGridX,int iGridY);

    // light computation
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
