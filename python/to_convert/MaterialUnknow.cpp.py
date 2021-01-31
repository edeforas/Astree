# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialUnknow.h"

#######################################
MaterialUnknow.MaterialUnknow()
    _sName="Unknow"
    _sFormula="Unknow"
    _iSolidColor=0xf0f0f0

#######################################
MaterialUnknow.MaterialUnknow( MaterialUnknow& m):  Glass(m)
{
#######################################
def clone(self):
    return MaterialUnknow(*self)

#######################################
def compute_index(self, dLambdaMicrons):
    (void)dLambdaMicrons
    return 1.

#######################################
