// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef MaterialWater_
#define MaterialWater_

#include "Glass.h"

class MaterialWater : public Glass
{
public:
    MaterialWater();
    MaterialWater(const MaterialWater& m);

    virtual Glass* clone() const override;

protected:
    virtual double compute_index(double dLambdaMicrons) override;
};

#endif
