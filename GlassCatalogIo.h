// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef GlassCatalogIO_
#define GlassCatalogIO_

#include <string>
using namespace std;

class GlassManager;

class GlassCatalogIO
{
public:
    static bool load(string sFile, GlassManager &pManager);
};

#endif
