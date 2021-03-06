// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialVacuum.h"

//////////////////////////////////////////////////////////////////////////////
MaterialVacuum::MaterialVacuum()
{
    _sName="Vacuum";
    _sFormula="Constant";
    set_maker("internal");
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
double MaterialVacuum::compute_index(double dLambdaMicrons)
{
    (void)dLambdaMicrons;
    return 1.;
}
//////////////////////////////////////////////////////////////////////////////
