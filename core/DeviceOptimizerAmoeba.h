// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceOptimizerAmoeba_
#define DeviceOptimizerAmoeba_

#include "DeviceOptimizer.h"

#include <string>
#include <vector>
using namespace std;

#define AMOEBA_MAX_ITER 500

class DeviceOptimizerAmoeba : public DeviceOptimizer
{
public:
    DeviceOptimizerAmoeba();
	virtual ~DeviceOptimizerAmoeba();

    virtual OptimizerResult optimize() override;
};

#endif
