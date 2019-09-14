// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef GlassExtended2_
#define GlassExtended2_

#include "Glass.h"

// glass definition as in the https://github.com/nzhagen/zemaxglass

class GlassExtended2 : public Glass
{
public:
    GlassExtended2();
    GlassExtended2(const GlassExtended2& m);
    virtual Glass* clone() const override;

    void set_coefs(double dC0,double dC1,double dC2,double dC3,double dC4,double dC5,double dC6,double dC7);

protected:
    virtual double compute_index(double dLambdaMicrons) override;

private:
    double _dC0;
    double _dC1;
    double _dC2;
    double _dC3;
    double _dC4;
    double _dC5;
    double _dC6;
    double _dC7;
};

#endif
