// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceIoZmx_
#define DeviceIoZmx_

#include <string>
using namespace std;

class OpticalDevice;

class DeviceIoZmx
{
public:
    static OpticalDevice* import(string sFile);
};

#endif
