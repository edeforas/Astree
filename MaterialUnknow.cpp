// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialUnknow.h"

//////////////////////////////////////////////////////////////////////////////
MaterialUnknow::MaterialUnknow()
{
    _sName="Unknow";
    _sFormulae="Unknow";
    _iSolidColor=0xf0f0f0;
}
//////////////////////////////////////////////////////////////////////////////
MaterialUnknow::MaterialUnknow(const MaterialUnknow& m):  Material(m)
{ }
//////////////////////////////////////////////////////////////////////////////
Material* MaterialUnknow::clone() const
{
    return new MaterialUnknow(*this);
}
//////////////////////////////////////////////////////////////////////////////
double MaterialUnknow::calc_index(double dLambdaMicrons)
{
    (void)dLambdaMicrons;
    return 1.;
}
//////////////////////////////////////////////////////////////////////////////
