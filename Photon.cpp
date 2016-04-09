// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "Photon.h"
#include <cmath>
#include <cassert>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
Photon::Photon()
{
    x=0.;
    y=0.;
    z=0.;

    dx=0.;
    dy=0.;
    dz=0.;

    _lambda=0.;

    valid=false;
}
////////////////////////////////////////////////////////////////////////////////
void Photon::set_lambda(double dLambda)
{
    assert(dLambda>0.);
    _lambda=dLambda;
}
////////////////////////////////////////////////////////////////////////////////
double Photon::lambda() const
{
    assert(_lambda>0.);
    return _lambda;
}
////////////////////////////////////////////////////////////////////////////////
bool Photon::is_valid() const
{
    assert(!isnan(x));
    assert(!isnan(y));
    assert(!isnan(z));
    assert(!isnan(dx));
    assert(!isnan(dy));
    assert(!isnan(dz));

    assert(!isinf(x));
    assert(!isinf(y));
    assert(!isinf(z));
    assert(!isinf(dx));
    assert(!isinf(dy));
    assert(!isinf(dz));

   // assert(_lambda>0.);

    return valid;
}
////////////////////////////////////////////////////////////////////////////////

