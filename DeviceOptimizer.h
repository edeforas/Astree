// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceOptimizer_
#define DeviceOptimizer_

#include <string>
#include <vector>
using namespace std;

#include "OpticalDevice.h"

#define MIN_RESOLUTION_CONIC 1.e-7
#define MIN_RESOLUTION_RCURV 1.e-7
#define MIN_RESOLUTION_CURVATURE 1.e-7 //TODO?
#define MIN_RESOLUTION_THICK 1.e-7

#define MIN_RESOLUTION_R4 1.e-22
#define MIN_RESOLUTION_R6 1.e-24
#define MIN_RESOLUTION_R8 1.e-26
#define MIN_RESOLUTION_R10 1.e-28

#define AMOEBA_MAX_ITER 500

class DeviceOptimizerParameter
{
public:
    int iSurface;
    string sParameter;
    double dMin;
    double dMax;
    double dVal;
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
    void set_device(OpticalDevice* pDevice);
    void add_parameter(int iSurface,string sParameter,double dMin,double dMax);
    OptimizerResult optimise_random(OptimizerMeritFunction eMeritFunction);
    OptimizerResult optimise_amoeba(OptimizerMeritFunction eMeritFunction);

private:
    void apply_parameter(const vector<DeviceOptimizerParameter>& parameters);
    double compute_demerit(OptimizerMeritFunction eMeritFunction); //return demerit value: lower is better

    OpticalDevice* _pDevice;
    ParameterSet _parameters;
};

#endif
