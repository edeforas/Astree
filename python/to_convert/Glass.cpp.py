# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "Glass.h"

#######################################
Glass.Glass():
    _sMaker("unknow"),
    _sName("unknow"),
    _sFormula("unknow"),
    _iSolidColor(0),
    _dLastLambda(0.),
    _dLastIndex(-1.)
{
#######################################
Glass.Glass( Glass& m):
    _sMaker(m._sMaker),
    _sName(m._sName),
    _sFormula(m._sFormula)
    _iSolidColor=m._iSolidColor
    _dLastIndex=-1.
    _dLastLambda=0.

#######################################
Glass.~Glass()
{
#######################################
def index(self, dLambdaMicrons):
    if dLambdaMicrons!=_dLastLambda:
        _dLastLambda=dLambdaMicrons
        _dLastIndex= compute_index(dLambdaMicrons)


    return _dLastIndex

#######################################
def maker(self):
    return _sMaker

#######################################
def set_maker(self, sMaker):
    _sMaker=sMaker

#######################################
def name(self):
    return _sName

#######################################
def set_name(self, sName):
    _sName=sName

#######################################
def set_formula(self, sFormula):
    _sFormula=sFormula

#######################################
def solid_color(self):
    return _iSolidColor

#######################################
def set_solid_color(self, iSolidColor):
    _iSolidColor=iSolidColor

#######################################
def formula(self):
    return _sFormula

#######################################
def compute_NdVd(self, &Nd, Vd):
    double dLambdaD=587.5618e-3; # He in microns
    double dLambdaF=486.13e-3; # H in microns
    double dLambdaC=656.27e-3; # H in microns

    Nd=index(dLambdaD)
    double Nf=index(dLambdaF)
    double Nc=index(dLambdaC)

    if Nf!=Nc:
        Vd=(Nd-1.)/(Nf-Nc)
    else:
        Vd=-1.; #not applicable

#######################################
def compute_NeVe(self, &Ne, Ve):
    double dLambdae=546.07e-3; # Hg in microns
    double dLambdaFp=479.99e-3; # Cd in microns
    double dLambdaCp=643.85e-3; # Cd in microns

    Ne=index(dLambdae)
    double Nfp=index(dLambdaFp)
    double Ncp=index(dLambdaCp)

    if Nfp!=Ncp:
        Ve=(Ne-1.)/(Nfp-Ncp)
    else:
        Ve=-1.; #not applicable

#######################################
def compute_solid_color(self):
	double Nd, Vd
	double t
	compute_NdVd(Nd, Vd)

	# crown color
	double dRed90=150
	double dGreen90=255
	double dBlue90=255

	#flint color
	double dRed20=30
	double dGreen20=150
	double dBlue20=255

	#interpolation crown-flint
	t = (90.-Vd)/(90.-20.)
	if t > 1.:
		t=1.
	if t < 0.:
		t=0.
	red = dRed90 * t + (1. - t)*dRed20
	green = dGreen90 * t + (1. - t)*dGreen20
	blue = dBlue90 * t + (1. - t)*dBlue20
	_iSolidColor = (int)(((int)red<<16)+ ((int)green << 8)+ ((int)blue))

#######################################