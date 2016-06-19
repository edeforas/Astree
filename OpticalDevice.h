// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _OpticalDevice_
#define _OpticalDevice_

#include <string>
#include <vector>
#include <map>
using namespace std;

#include "ImageQuality.h"
#include "Surface.h"
class Light;

#define RADIUS_CURVATURE_INFINITY 9e99
#define FD_VALID_MAX 1e10
#define SPOT_SIZE_INFINITY 9e99

enum eSurfaceparameter //to be used with get/set functions below
{
    DIAMETER,
    AUTO_DIAMETER, // set with 0 or 1
    INNER_DIAMETER,
    AUTO_INNER_DIAMETER, // set with 0 or 1
    CONIC,
    RADIUS_CURVATURE, //in mm
    CURVATURE, //inverse of radius curvature, unit is 1/mm
    Z, //absolute coord
    THICK, //relative coord
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
    void set_relative_convention(bool bRelativeConvention);
    bool relative_convention() const;

    void set_autofocus(bool bAutofocus);
    bool get_autofocus();
    void set_image_autocurvature(bool bAutoCurvature);
    bool get_image_autocurvature();

    bool compute_surface_profile(int iSurface,double dX,double dY,double& dZ); //todo add slope computation

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

    //other parameters, not used in computation, saved in files
    void set_parameter(const string& sKey,double dValue);
    void set_parameter(const string& sKey,const string& sValue);
    bool get_parameter(const string& sKey,double & dValue);
    bool get_parameter(const string& sKey,string & sValue);
    const map<string,string>& all_parameters() const;

private:
    void initialize_light(Light *pLight, double dTilt, int iGridX, int iGridY);
    void ray_trace();
    void ray_trace_step(Light &light, double dTilt, bool bAutofocus, bool bAutocurvature);
    void update_z();
    void update_thicks();

    vector<double> _vdThicks;
    vector<Surface> _vSurfaces;
    bool _bRelativeConvention;
    bool _bAutoCurvature;
    bool _bAutoFocus;

    string _sNote;

    double _dHalfFov;
    int _iGridX,_iGridY;
    int _iNbAngles;
    string _sLightColors;
    bool _bMustRetrace;
    ImageQuality _imageQuality;

    // other parameters
    map<string,string> _otherParameters;
};

#endif
