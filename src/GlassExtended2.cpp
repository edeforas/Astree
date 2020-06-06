// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "GlassExtended2.h"
#include <cmath>
#include <cassert>

// glass definition as in the https://github.com/nzhagen/zemaxglass

//////////////////////////////////////////////////////////////////////////////
GlassExtended2::GlassExtended2()
{
    _sName="Extended2";
    _sFormula="Extended2";

    _dC0=0.;
    _dC1=0.;
    _dC2=0.;
    _dC3=0.;
    _dC4=0.;
    _dC5=0.;
    _dC6=0.;
    _dC7=0.;
}
//////////////////////////////////////////////////////////////////////////////
Glass* GlassExtended2::clone() const
{
    return new GlassExtended2(*this);
}
//////////////////////////////////////////////////////////////////////////////
GlassExtended2::GlassExtended2(const GlassExtended2& m):
    Glass(m)
{
    assert(m._sFormula=="Extended2");

    _dC0=m._dC0;
    _dC1=m._dC1;
    _dC2=m._dC2;
    _dC3=m._dC3;
    _dC4=m._dC4;
    _dC5=m._dC5;
    _dC6=m._dC6;
    _dC7=m._dC7;
}
//////////////////////////////////////////////////////////////////////////////
double GlassExtended2::compute_index(double dLambdaMicrons)
{
    //use the Extended2 formulae
    //	n^2 = c0 + c1 λ^2 + c2 λ^−2 + c3 λ^−4 + c4 λ^−6 + c5 λ^−8 + c6 λ^4 + c7 λ^6
    
	double l2=dLambdaMicrons*dLambdaMicrons;
	double l4=l2*l2;
	double invl2=1./l2;
	double invl4=invl2*invl2;
	
    //todo optimize with the Horner rule
    double n2=_dC0+_dC1*l2+_dC2*invl2+_dC3*invl4+_dC4*invl2*invl4+_dC5*invl4*invl4+_dC6*l4+_dC7*l4*l2;
    return sqrt(n2);
}
//////////////////////////////////////////////////////////////////////////////
void GlassExtended2::set_coefs(double dC0,double dC1,double dC2,double dC3,double dC4,double dC5,double dC6,double dC7)
{
    _dC0=dC0;
    _dC1=dC1;
    _dC2=dC2;
    _dC3=dC3;
    _dC4=dC4;
    _dC5=dC5;
    _dC6=dC6;
    _dC7=dC7;
}
//////////////////////////////////////////////////////////////////////////////
