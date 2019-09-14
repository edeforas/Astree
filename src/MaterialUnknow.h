// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef MaterialUnknow_
#define MaterialUnknow_

#include "Glass.h"

class MaterialUnknow : public Glass
{
public:
    MaterialUnknow();
    MaterialUnknow(const MaterialUnknow& m);

    virtual Glass* clone() const override;

protected:
    virtual double compute_index(double dLambda) override;
};

#endif
