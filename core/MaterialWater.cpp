// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialWater.h"

//////////////////////////////////////////////////////////////////////////////
MaterialWater::MaterialWater()
{
    _sName="Water";
    _sFormula="Constant";
    set_maker("internal");
    _iSolidColor=0xf0f0f0;
}
//////////////////////////////////////////////////////////////////////////////
MaterialWater::MaterialWater(const MaterialWater& m):  Glass(m)
{ }
//////////////////////////////////////////////////////////////////////////////
Glass* MaterialWater::clone() const
{
    return new MaterialWater(*this);
}
//////////////////////////////////////////////////////////////////////////////
double MaterialWater::compute_index(double dLambdaMicrons)
{
    (void)dLambdaMicrons;
    return 1.333;
}
//////////////////////////////////////////////////////////////////////////////
