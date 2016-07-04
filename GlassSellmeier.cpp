// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "GlassSellmeier.h"
#include <cmath>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////
GlassSellmeier::GlassSellmeier()
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
Glass* GlassSellmeier::clone() const
{
    return new GlassSellmeier(*this);
}
//////////////////////////////////////////////////////////////////////////////
GlassSellmeier::GlassSellmeier(const GlassSellmeier& m):
    Glass(m)
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
double GlassSellmeier::calc_index(double dLambdaMicrons)
{
    //use of the Selmeier formulae
    double l2=dLambdaMicrons*dLambdaMicrons;

    return sqrt( 1.+_B1*l2/(l2-_C1)+_B2*l2/(l2-_C2)+_B3*l2/(l2-_C3) );
}
//////////////////////////////////////////////////////////////////////////////
void GlassSellmeier::set_coefs(double B1,double B2,double B3,double C1,double C2,double C3)
{
    _B1=B1;
    _B2=B2;
    _B3=B3;
    _C1=C1;
    _C2=C2;
    _C3=C3;
}
//////////////////////////////////////////////////////////////////////////////
