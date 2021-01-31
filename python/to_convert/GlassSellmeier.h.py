# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef GlassSellmeier_
#define GlassSellmeier_

#include "Glass.h"

class GlassSellmeier : public Glass
public:
    GlassSellmeier()
    GlassSellmeier( GlassSellmeier& m)
    virtual Glass* clone()  override

    void set_coefs(double dB1, dB2, dB3, dC1, dC2, dC3)

protected:
    virtual double compute_index(double dLambdaMicrons) override

private:
    double _dB1
    double _dB2
    double _dB3
    double _dC1
    double _dC2
    double _dC3


#endif
