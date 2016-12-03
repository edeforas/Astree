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

    void set_coefs(double B1,double B2,double B3,double C1,double C2,double C3);

protected:
    virtual double calc_index(double dLambdaMicrons);

private:
    double _B1;
    double _B2;
    double _B3;
    double _C1;
    double _C2;
    double _C3;
};

#endif
