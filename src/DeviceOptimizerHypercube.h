/// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceOptimizerHypercube_
#define DeviceOptimizerHypercube_

#include "DeviceOptimizer.h"

#define DIVIDE_ITER_BY_AXIS 5

class DeviceOptimizerHypercube : public DeviceOptimizer
{
public:
    DeviceOptimizerHypercube();
    virtual ~DeviceOptimizerHypercube();

    OptimizerResult optimize() override;
};

#endif
