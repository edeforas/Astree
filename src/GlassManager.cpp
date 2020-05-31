// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "GlassManager.h"
#include "Glass.h"
#include "MaterialAir.h"
#include "MaterialVacuum.h"
#include "MaterialWater.h"
#include "MaterialUnknow.h"

#include <cassert>

GlassManager* GlassManager::_pGlassManager=0;

//////////////////////////////////////////////////////////////////////////////
GlassManager::GlassManager()
{
    _vGlass.push_back(new MaterialAir);
    _vGlass.push_back(new MaterialVacuum);
    _vGlass.push_back(new MaterialWater);
}
//////////////////////////////////////////////////////////////////////////////
GlassManager::~GlassManager()
{
    for(unsigned int i=0;i<_vGlass.size();i++)
        delete _vGlass[i]; //TODO check destructor is called
}
//////////////////////////////////////////////////////////////////////////////
Glass* GlassManager::create(string sMaterial) const
{
    for(unsigned int i=0;i<_vGlass.size();i++)
    {
        if(_vGlass[i]->name()==sMaterial)
            return _vGlass[i]->clone();
    }

    //if glass does not exist , try to add N- behind(Schott lead free glasses)
    sMaterial="N-"+sMaterial;
    for(unsigned int i=0;i<_vGlass.size();i++)
    {
        if(_vGlass[i]->name()==sMaterial)
            return _vGlass[i]->clone();
    }

    //error case
    Glass* pM=new MaterialUnknow;
    pM->set_formula("unknown");
    pM->set_name(sMaterial);
    pM->set_maker("unknown_glass");
    return pM;
}
//////////////////////////////////////////////////////////////////////////////
void GlassManager::destroy(Glass* pMaterial)
{
    delete pMaterial;
}
//////////////////////////////////////////////////////////////////////////////
GlassManager& GlassManager::singleton()
{
    if(_pGlassManager==0)
        _pGlassManager= new GlassManager; //TODO delete at exit

    assert(_pGlassManager);
    return *_pGlassManager;
}
//////////////////////////////////////////////////////////////////////////////
void GlassManager::list_available(vector<string>& vsAvailable)
{
    vsAvailable.clear();
    for(unsigned int i=0;i<_vGlass.size();i++)
        vsAvailable.push_back(_vGlass[i]->name());
}
//////////////////////////////////////////////////////////////////////////////
bool GlassManager::exist(const string& sGlass) const
{
    for(unsigned int i=0;i<_vGlass.size();i++)
    {
        if(_vGlass[i]->name()==sGlass)
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////
unsigned int GlassManager::solid_color(const string& sMaterial)
{
    for(unsigned int i=0;i<_vGlass.size();i++)
    {
        if(_vGlass[i]->name()==sMaterial)
            return _vGlass[i]->solid_color();
    }
    return 0xffffff;
}
//////////////////////////////////////////////////////////////////////////////
void GlassManager::inject(Glass* pGlass) //take ownership of pGlass
{
    _vGlass.push_back(pGlass);
}
//////////////////////////////////////////////////////////////////////////////
