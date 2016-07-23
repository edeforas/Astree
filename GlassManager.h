// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _MaterialManager_
#define _MaterialManager_

#include <string>
#include <vector>
using namespace std;

class Glass;

class GlassManager
{
public:
    static GlassManager& singleton();
    Glass* create(string sMaterial) const;
    void destroy(Glass* pMaterial);

    void list_available(vector<string>& vsAvailable);

    unsigned int solid_color(string sMaterial);

    void inject(Glass* pGlass); //take ownership of pGlass
private:
    GlassManager();
    ~GlassManager();

    static GlassManager* _pGlassManager;
    vector<Glass*> _vGlass; // todo use map
};

#endif
