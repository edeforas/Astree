# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef MaterialVacuum_
#define MaterialVacuum_

#include "Glass.h"

class MaterialVacuum : public Glass
public:
    MaterialVacuum()
    MaterialVacuum( MaterialVacuum& m)

    virtual Glass* clone()  override

protected:
    virtual double compute_index(double dLambdaMicrons) override


#endif
