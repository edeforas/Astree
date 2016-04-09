// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _MaterialWater_
#define _MaterialWater_

#include "Material.h"

class MaterialWater : public Material
{
public:
    MaterialWater();
    MaterialWater(const MaterialWater& m);

    virtual Material* clone() const;

protected:
    virtual double calc_index(double dLambda);
};

#endif
