// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef GlassSellmeier_
#define GlassSellmeier_

#include "Glass.h"

class GlassSellmeier : public Glass
{
public:
    GlassSellmeier();
    GlassSellmeier(const GlassSellmeier& m);
    virtual Glass* clone() const;

    void set_coefs(double dB1,double dB2,double dB3,double dC1,double dC2,double dC3);

protected:
    virtual double calc_index(double dLambdaMicrons);

private:
    double _dB1;
    double _dB2;
    double _dB3;
    double _dC1;
    double _dC2;
    double _dC3;
};

#endif
