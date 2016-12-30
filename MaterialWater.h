// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef MaterialWater_
#define MaterialWater_

#include "Glass.h"

class MaterialWater : public Glass
{
public:
    MaterialWater();
    MaterialWater(const MaterialWater& m);

    virtual Glass* clone() const;

protected:
    virtual double calc_index(double dLambda);
};

#endif
