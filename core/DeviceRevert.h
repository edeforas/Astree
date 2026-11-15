// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceRevert_
#define DeviceRevert_

class OpticalDevice;

class DeviceRevert
{
public:
    bool revert(OpticalDevice* pDevice, int iFirstSurface, int iLastSurface);
};

#endif
