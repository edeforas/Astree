// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialWater.h"

//////////////////////////////////////////////////////////////////////////////
MaterialWater::MaterialWater()
{
    _sName="Water";
    _sFormulae="Constant";
    _iSolidColor=0xf0f0f0;
}
//////////////////////////////////////////////////////////////////////////////
MaterialWater::MaterialWater(const MaterialWater& m):  Material(m)
{ }
//////////////////////////////////////////////////////////////////////////////
Material* MaterialWater::clone() const
{
    return new MaterialWater(*this);
}
//////////////////////////////////////////////////////////////////////////////
double MaterialWater::calc_index(double dLambdaMicrons)
{
    (void)dLambdaMicrons;
    return 1.333;
}
//////////////////////////////////////////////////////////////////////////////
