# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MaterialAir.h"

#######################################
MaterialAir.MaterialAir()
    _sName="Air"
    _sFormula="Custom"
    set_maker("internal")
    _iSolidColor=0xffffff

#######################################
MaterialAir.MaterialAir( MaterialAir& m):  Glass(m)
{
#######################################
def clone(self):
    return MaterialAir(*self)

#######################################
def compute_index(self, dLambdaMicrons):
    double sigma2=1./(dLambdaMicrons*dLambdaMicrons); #dLambda in microns
    return 1. + 6.4328e-5 + 2.94981e-2/(146.-sigma2) + 2.554e-4/(41.- sigma2)

    #source : http:#olivier.fournet.free.fr/science_et_physique/refraction_air.htm
    #  with 0.03% de CO2 , Pa , T= 288.15 K

#######################################
