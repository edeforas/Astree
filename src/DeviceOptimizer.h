// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceOptimizer_
#define DeviceOptimizer_

#include <string>
#include <vector>
using namespace std;

#include "OpticalDevice.h"

#define MIN_RESOLUTION_CONIC 1.e-7
#define CONIC_HALF_RANGE 4
#define CONIC_HALF_RANGE_REFINE 0.5

#define MIN_RESOLUTION_RCURV 1.e-7
#define RCURV_HALF_RATIO 2
#define RCURV_HALF_RATIO_REFINE 1.05
#define MIN_RESOLUTION_CURVATURE 1.e-13 //TODO? !!

#define MIN_RESOLUTION_THICK 1.e-7
#define Z_HALF_RANGE 100.
#define THICK_HALF_RANGE 100.
#define Z_HALF_RANGE_REFINE 5
#define THICK_HALF_RANGE_REFINE 5

#define MIN_RESOLUTION_R4 1.e-22
#define R4_HALF_RATIO 2
#define R4_HALF_RATIO_REFINE 1.05

#define MIN_RESOLUTION_R6 1.e-24
#define R6_HALF_RATIO 2
#define R6_HALF_RATIO_REFINE 1.05

#define MIN_RESOLUTION_R8 1.e-26
#define R8_HALF_RATIO 2
#define R8_HALF_RATIO_REFINE 1.05

#define MIN_RESOLUTION_R10 1.e-28
#define R10_HALF_RATIO 2
#define R10_HALF_RATIO_REFINE 1.05

class DeviceOptimizerParameter
{
public:
    int iSurface;
    string sParameter;
    double dMin;
    double dMax;
    double dValue;
    double dResolution;
};

enum OptimizerMeritFunction
{
    eCenterOnly,
    eMostlyCenter,
    eFullFrameMean,
    eFullFrameMaxError
};

enum OptimizerResult
{
    eInvalidState,
    eBetterSolutionFound,
    eNoBetterSolution,
    eSolutionOnEdge,
    eNothingToOptimize
};

typedef vector<DeviceOptimizerParameter> ParameterSet;

class DeviceOptimizer
{
public:
    DeviceOptimizer();
    virtual ~DeviceOptimizer();

    void clear();
    void set_device(OpticalDevice* pDevice);
    void set_min_vignetting(double dMinVignetting);
    void add_parameter(int iSurface,const string& sParameter, bool bRefine=false); // auto min & max
    void add_parameter(int iSurface,const string& sParameter,double dMin,double dMax);
    void set_merit_function(OptimizerMeritFunction eMeritFunction);

    virtual OptimizerResult optimize()=0;

    bool domain_under_resolution(const ParameterSet& params);

protected:
    void apply_parameter(const ParameterSet& parameters);
    double compute_demerit(); //return demerit value: lower is better

    OpticalDevice* _pDevice;
    ParameterSet _parameters;
    OptimizerMeritFunction _meritFunction;
    double _dMinVignetting;
};

#endif
