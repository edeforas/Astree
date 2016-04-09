// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _MaterialVacuum_
#define _MaterialVacuum_

#include "Material.h"

class MaterialVacuum : public Material
{
public:
    MaterialVacuum();
    MaterialVacuum(const MaterialVacuum& m);

    virtual Material* clone() const;

protected:
    virtual double calc_index(double dLambda);
};

#endif
