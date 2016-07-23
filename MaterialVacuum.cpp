// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialVacuum.h"

//////////////////////////////////////////////////////////////////////////////
MaterialVacuum::MaterialVacuum()
{
    _sName="Vacuum";
    _sFormula="Constant";
    _iSolidColor=0xf0f0f0;
}
//////////////////////////////////////////////////////////////////////////////
MaterialVacuum::MaterialVacuum(const MaterialVacuum& m):  Glass(m)
{ }
//////////////////////////////////////////////////////////////////////////////
Glass* MaterialVacuum::clone() const
{
    return new MaterialVacuum(*this);
}
//////////////////////////////////////////////////////////////////////////////
double MaterialVacuum::calc_index(double dLambdaMicrons)
{
    (void)dLambdaMicrons;
    return 1.;
}
//////////////////////////////////////////////////////////////////////////////
