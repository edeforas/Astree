#/ self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceOptimizerRandom_
#define DeviceOptimizerRandom_

#include "DeviceOptimizer.h"

#define RANDOM_ITER_BY_AXIS 10

class DeviceOptimizerRandom : public DeviceOptimizer
public:
    DeviceOptimizerRandom()
    virtual ~DeviceOptimizerRandom()

    OptimizerResult optimize() override


#endif
