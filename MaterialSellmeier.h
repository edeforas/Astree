// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _MaterialSellmeier_
#define _MaterialSellmeier_

#include "Material.h"

class MaterialSellmeier : public Material
{
public:
    MaterialSellmeier();
    MaterialSellmeier(const MaterialSellmeier& m);

    virtual Material* clone() const;

    double _B1;
    double _B2;
    double _B3;
    double _C1;
    double _C2;
    double _C3;

protected:
    virtual double calc_index(double dLambdaMicrons);
};

#endif
