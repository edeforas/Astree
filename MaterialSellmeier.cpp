// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialSellmeier.h"
#include <cmath>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////
MaterialSellmeier::MaterialSellmeier()
{
    _sName="Sellmeier";
    _sFormulae="Sellmeier";

    _B1=0.;
    _B2=0.;
    _B3=0.;
    _C1=0.;
    _C2=0.;
    _C3=0.;
}
//////////////////////////////////////////////////////////////////////////////
Material* MaterialSellmeier::clone() const
{
    return new MaterialSellmeier(*this);
}
//////////////////////////////////////////////////////////////////////////////
MaterialSellmeier::MaterialSellmeier(const MaterialSellmeier& m):
    Material(m)
{
    assert(m._sFormulae=="Sellmeier");

    _B1=m._B1;
    _B2=m._B2;
    _B3=m._B3;
    _C1=m._C1;
    _C2=m._C2;
    _C3=m._C3;
}
//////////////////////////////////////////////////////////////////////////////
double MaterialSellmeier::calc_index(double dLambdaMicrons)
{
    //use of the Selmeier formulae
    double l2=dLambdaMicrons*dLambdaMicrons;

    return sqrt( 1.+_B1*l2/(l2-_C1)+_B2*l2/(l2-_C2)+_B3*l2/(l2-_C3) );
}//////////////////////////////////////////////////////////////////////////////
