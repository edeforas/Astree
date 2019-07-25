// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceOptimizerRandom_
#define DeviceOptimizerRandom_

#include "DeviceOptimizer.h"

#define RANDOM_ITER_BY_AXIS 10

class DeviceOptimizerRandom : public DeviceOptimizer
{
public:
    DeviceOptimizerRandom();
    virtual ~DeviceOptimizerRandom();

    OptimizerResult optimize();
};

#endif
