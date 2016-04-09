// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _DeviceScaling_
#define _DeviceScaling_

class OpticalDevice;

class DeviceScaling
{
public:
    bool scale(OpticalDevice* pDevice,double dRatio);
};

#endif
