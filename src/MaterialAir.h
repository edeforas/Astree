// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef MaterialAir_
#define MaterialAir_

#include "Glass.h"

class MaterialAir : public Glass
{
public:
    MaterialAir();
    MaterialAir(const MaterialAir& m);

    virtual Glass* clone() const;

protected:
    virtual double calc_index(double dLambda);
};

#endif
