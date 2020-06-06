// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "GlassManager.h"
#include "Glass.h"
#include "MaterialAir.h"
#include "MaterialVacuum.h"
#include "MaterialWater.h"
#include "MaterialUnknow.h"

#include <cassert>
#include <algorithm>

GlassManager* GlassManager::_pGlassManager=0;

//////////////////////////////////////////////////////////////////////////////
GlassManager::GlassManager()
{
	inject(new MaterialAir);
	inject(new MaterialVacuum);
	inject(new MaterialWater);
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
	string sMaterialOrig = sMaterial;
	if (sMaterial.find("N-") == string::npos)
	{
		sMaterial = "N-" + sMaterial;
		for (unsigned int i = 0; i < _vGlass.size(); i++)
		{
			if (_vGlass[i]->name() == sMaterial)
				return _vGlass[i]->clone();
		}
	}
    //error case
    Glass* pM=new MaterialUnknow;
    pM->set_formula("unknown");
    pM->set_name(sMaterialOrig);
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
void GlassManager::list_catalogs(vector<string>& vsCatalogs)
{
	vsCatalogs.clear();
	for (unsigned int i = 0; i < _vCatalogs.size(); i++)
		vsCatalogs.push_back(_vCatalogs[i]);
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

	//add maker in catalog if no existent
	auto it = std::find(_vCatalogs.begin(), _vCatalogs.end(), pGlass->maker());
	if (it == _vCatalogs.end())
		_vCatalogs.push_back(pGlass->maker());
}
//////////////////////////////////////////////////////////////////////////////
