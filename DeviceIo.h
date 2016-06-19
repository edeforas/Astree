// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _DeviceIo_
#define _DeviceIo_

#include <string>
using namespace std;

class OpticalDevice;
class Properties;

class DeviceIo
{
public:
    static bool save(string sFile, OpticalDevice* pD);
    static OpticalDevice* load(string sFile);
};

#endif
