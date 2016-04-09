// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialManager.h"
#include "Material.h"
#include "MaterialAir.h"
#include "MaterialVacuum.h"
#include "MaterialWater.h"
#include "MaterialUnknow.h"
#include "MaterialIo.h"

#include <cassert>

MaterialManager* MaterialManager::_pMaterialManager=0;

//////////////////////////////////////////////////////////////////////////////
MaterialManager::MaterialManager()
{
    _vMaterial.push_back(new MaterialAir);
    _vMaterial.push_back(new MaterialVacuum);
    _vMaterial.push_back(new MaterialWater);
}
//////////////////////////////////////////////////////////////////////////////
MaterialManager::~MaterialManager()
{
    // TODO delete _vMaterial
}
//////////////////////////////////////////////////////////////////////////////
Material* MaterialManager::create(string sMaterial) const
{
    for(unsigned int i=0;i<_vMaterial.size();i++)
    {
        if(_vMaterial[i]->name()==sMaterial)
            return _vMaterial[i]->clone();
    }

    //error case
    Material* pM=new MaterialUnknow;
    pM->set_formulae("placeholder for "+sMaterial);
    return pM;
}
//////////////////////////////////////////////////////////////////////////////
void MaterialManager::destroy(Material* pMaterial)
{
    delete pMaterial;
}
//////////////////////////////////////////////////////////////////////////////
MaterialManager& MaterialManager::singleton()
{
    if(_pMaterialManager==0)
        _pMaterialManager= new MaterialManager; //TODO delete at exit

    assert(_pMaterialManager);
    return *_pMaterialManager;
}
//////////////////////////////////////////////////////////////////////////////
void MaterialManager::list_available(vector<string>& vsAvailable)
{
    vsAvailable.clear();
    for(unsigned int i=0;i<_vMaterial.size();i++)
        vsAvailable.push_back(_vMaterial[i]->name());
}
//////////////////////////////////////////////////////////////////////////////
bool MaterialManager::load(string sFile)
{
    Material* pMat=MaterialIo::load(sFile);
    if(pMat==0)
        return false;

    //TODO verifier existence avant ajout
    _vMaterial.push_back(pMat);
    return true;
}
//////////////////////////////////////////////////////////////////////////////
unsigned int MaterialManager::solid_color(string sMaterial)
{
    for(unsigned int i=0;i<_vMaterial.size();i++)
    {
        if(_vMaterial[i]->name()==sMaterial)
            return _vMaterial[i]->solid_color();
    }
    return 0xffffff;
}
//////////////////////////////////////////////////////////////////////////////
