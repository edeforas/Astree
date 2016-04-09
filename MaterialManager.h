// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _MaterialManager_
#define _MaterialManager_

#include <string>
#include <vector>
using namespace std;

class Material;

class MaterialManager
{
public:
    static MaterialManager& singleton();
    Material* create(string sMaterial) const;
    void destroy(Material* pMaterial);

    void list_available(vector<string>& vsAvailable);

    bool load(string sFile);
    unsigned int solid_color(string sMaterial);

private:
    MaterialManager();
    ~MaterialManager();

    static MaterialManager* _pMaterialManager;
    vector<Material*> _vMaterial;
};

#endif
