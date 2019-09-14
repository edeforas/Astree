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
    _sFormula="Sellmeier";

    _dB1=0.;
    _dB2=0.;
    _dB3=0.;
    _dC1=0.;
    _dC2=0.;
    _dC3=0.;
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
    assert(m._sFormula=="Sellmeier");

    _dB1=m._dB1;
    _dB2=m._dB2;
    _dB3=m._dB3;
    _dC1=m._dC1;
    _dC2=m._dC2;
    _dC3=m._dC3;
}
//////////////////////////////////////////////////////////////////////////////
double GlassSellmeier::compute_index(double dLambdaMicrons)
{
    //use the Selmeier formulae
    double l2=dLambdaMicrons*dLambdaMicrons;
    return sqrt( 1.+_dB1*l2/(l2-_dC1)+_dB2*l2/(l2-_dC2)+_dB3*l2/(l2-_dC3) );
}
//////////////////////////////////////////////////////////////////////////////
void GlassSellmeier::set_coefs(double dB1,double dB2,double dB3,double dC1,double dC2,double dC3)
{
    _dB1=dB1;
    _dB2=dB2;
    _dB3=dB3;
    _dC1=dC1;
    _dC2=dC2;
    _dC3=dC3;
}
//////////////////////////////////////////////////////////////////////////////
