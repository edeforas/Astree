# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef OpticalDevice_
#define OpticalDevice_

#include <string>
#include <vector>
#include <map>
using namespace std

#include "ImageQuality.h"
#include "Surface.h"
class Light

#define RADIUS_CURVATURE_INFINITY 9e99
#define FD_VALID_MAX 1e10
#define SPOT_SIZE_INFINITY 9e99

#define NB_PHOTON_X (35)
#define NB_PHOTON_Y (35)

enum eSurfaceParameter #to be used with get/set functions below
    DIAMETER,
    AUTO_DIAMETER, # set with 0 or 1
    INNER_DIAMETER,
    AUTO_INNER_DIAMETER, # set with 0 or 1
    CONIC,
    RADIUS_CURVATURE, #in mm
    CURVATURE, #inverse of radius curvature, is 1/mm
    Z, #absolute coordinate
    THICK, #relative coordinate
    R4, #surface polynomial coef of r4
    R6, #surface polynomial coef of r6
    R8, #surface polynomial coef of r8
    R10 #surface polynomial coef of r10


struct SurfaceParameterClone
     eSurfaceParameter  param
     int iSurface
     int iRefSurface
     double dGain


class OpticalDevice
public:   
    OpticalDevice()
    OpticalDevice( OpticalDevice &rDevice)
    OpticalDevice& operator=( OpticalDevice& rDevice)
    virtual ~OpticalDevice()

    # surface management
    void insert_surface(int iPos)
    void delete_surface(int iPos)
    int nb_surface()

    # surface type management
    void set_type(int iSurf, sType)
    string type(int iSurf)

    # note management
    void set_note( string& sNote)
    string note()

    # surface data accessor
    void set(int iSurface, eParam, dParam)
    double get(int iSurface, eParam, bUpdate=True); #not  due to lazy computation

    # surface parameter cloning
    void set_clone(int iSurface, eParam, iRefSurface, dGain); #set iRefSurface=-1 to remove
    bool is_clone(int iSurface, eParam) const; #return False if no clone for iSurface eParam
    bool get_clone(int iSurface, eParam, iRefSurface, &dGain) const; #return False if no exist

    # global properties getter
    bool has_aspheric()
    bool has_inner_diameter()
    bool has_auto()
    bool is_image_infinite()

    # z settings
    void set_relative_convention(bool bRelativeConvention)
    bool relative_convention()

    void set_autofocus(bool bAutofocus)
    bool get_autofocus()
    void set_image_autocurvature(bool bAutoCurvature)
    bool get_image_autocurvature()

    bool compute_surface_profile(int iSurface, dX, dY, dZ); #TODO add slope computation

    # comment settings
    void set_comment(int iSurface, sComment)
    string comment(int iSurface)
    bool has_comment()

    # input light properties
    void set_half_field_of_view(double dHalfFov)
    double half_field_of_view()
    int nb_intermediate_angles()
    void set_nb_intermediate_angles(int iNbAngles)
    void set_light_colors( string & _sLightColors)
    string light_colors()
    void set_light_grid(int iGridX, iGridY)

    # light computation
    void compute_light(Light* pLight, iSurface, dTilt, iGridX, iGridY)
     ImageQuality get_image_quality()

    #other parameters, used in computation, in files
    void set_parameter( string& sKey, dValue)
    void set_parameter( string& sKey, sValue)
    bool get_parameter( string& sKey, & dValue)
    bool get_parameter( string& sKey, & sValue)
     map<string, all_parameters()

private:
    void initialize_light(Light *pLight, dTilt, iGridX, iGridY)
    void ray_trace()
    void ray_trace_step(Light &light, dTilt, bAutofocus, bAutocurvature)
    void update_z()
    void update_thicks()

    vector<double> _vdThicks
    vector<Surface> _vSurfaces
    bool _bRelativeConvention
    bool _bAutoCurvature
    bool _bAutoFocus

    string _sNote

    double _dHalfFov
    int _iGridX,_iGridY
    int _iNbAngles
    string _sLightColors
    bool _bMustRetrace

    ImageQuality _imageQuality

    # other parameters
    map<string, _otherParameters

    #surface clone parameters
    vector<SurfaceParameterClone> _surfParamsClone


#endif
