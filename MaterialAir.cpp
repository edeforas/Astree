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
double MaterialAir::calc_index(double dLambdaMicrons)
{
    double sigma2=1./(dLambdaMicrons*dLambdaMicrons); //dLambda en microns
    return 1. + 6.4328e-5 + 2.94981e-2/(146.-sigma2) + 2.554e-4/(41.- sigma2);

    //source : http://olivier.fournet.free.fr/science_et_physique/refraction_air.htm

    //condition standard de temperature et de pression:
    //  avec 0.03% de CO2 (anhydride carbonique)
    //  et 101325 Pa (760 millimetres de mercure) et d'une temperature de 288,15 degK (15 degC).
}
//////////////////////////////////////////////////////////////////////////////
