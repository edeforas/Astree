// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _MaterialUnknow_
#define _MaterialUnknow_

#include "Material.h"

class MaterialUnknow : public Material
{
public:
    MaterialUnknow();
    MaterialUnknow(const MaterialUnknow& m);

    virtual Material* clone() const;

protected:
    virtual double calc_index(double dLambda);
};

#endif
