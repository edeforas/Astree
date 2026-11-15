// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceIo_
#define DeviceIo_

#include <string>
using namespace std;

class OpticalDevice;
class Properties;

class DeviceIo
{
public:
    static bool save(const string& sFile, OpticalDevice* pD);
    static OpticalDevice* load(const string& sFile);
};

#endif
