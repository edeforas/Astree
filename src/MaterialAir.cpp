// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialAir.h"

//////////////////////////////////////////////////////////////////////////////
MaterialAir::MaterialAir()
{
    _sName="Air";
    _sFormula="Custom";
    set_maker("internal");
    _iSolidColor=0xffffff;
}
//////////////////////////////////////////////////////////////////////////////
MaterialAir::MaterialAir(const MaterialAir& m):  Glass(m)
{ }
//////////////////////////////////////////////////////////////////////////////
Glass* MaterialAir::clone() const
{
    return new MaterialAir(*this);
}
//////////////////////////////////////////////////////////////////////////////
double MaterialAir::compute_index(double dLambdaMicrons)
{
    double sigma2=1./(dLambdaMicrons*dLambdaMicrons); //dLambda in microns
    return 1. + 6.4328e-5 + 2.94981e-2/(146.-sigma2) + 2.554e-4/(41.- sigma2);

    //source : http://olivier.fournet.free.fr/science_et_physique/refraction_air.htm
    //  with 0.03% de CO2 , 101325 Pa , T= 288.15 K
}
//////////////////////////////////////////////////////////////////////////////
