# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef GlassExtended2_
#define GlassExtended2_

#include "Glass.h"

# glass definition as in the https:#github.com/nzhagen/zemaxglass

class GlassExtended2 : public Glass
public:
    GlassExtended2()
    GlassExtended2( GlassExtended2& m)
    virtual Glass* clone()  override

    void set_coefs(double dC0, dC1, dC2, dC3, dC4, dC5, dC6, dC7)

protected:
    virtual double compute_index(double dLambdaMicrons) override

private:
    double _dC0
    double _dC1
    double _dC2
    double _dC3
    double _dC4
    double _dC5
    double _dC6
    double _dC7


#endif
