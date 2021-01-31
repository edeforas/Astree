# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "GlassSellmeier.h"
#include <cmath>
#include <cassert>

#######################################
GlassSellmeier.GlassSellmeier()
    _sName="Sellmeier"
    _sFormula="Sellmeier"

    _dB1=0.
    _dB2=0.
    _dB3=0.
    _dC1=0.
    _dC2=0.
    _dC3=0.

#######################################
def clone(self):
    return GlassSellmeier(*self)

#######################################
GlassSellmeier.GlassSellmeier( GlassSellmeier& m):
    Glass(m)
    assert(m._sFormula=="Sellmeier")

    _dB1=m._dB1
    _dB2=m._dB2
    _dB3=m._dB3
    _dC1=m._dC1
    _dC2=m._dC2
    _dC3=m._dC3

#######################################
def compute_index(self, dLambdaMicrons):
    #use the Selmeier formulae
    double l2=dLambdaMicrons*dLambdaMicrons
    return sqrt( 1.+_dB1*l2/(l2-_dC1)+_dB2*l2/(l2-_dC2)+_dB3*l2/(l2-_dC3) )

#######################################
def set_coefs(self, dB1, dB2, dB3, dC1, dC2, dC3):
    _dB1=dB1
    _dB2=dB2
    _dB3=dB3
    _dC1=dC1
    _dC2=dC2
    _dC3=dC3
	
	compute_solid_color()

#######################################
