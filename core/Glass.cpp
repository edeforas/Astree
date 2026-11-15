// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "Glass.h"

//////////////////////////////////////////////////////////////////////////////
Glass::Glass():
    _sMaker("unknow"),
    _sName("unknow"),
    _sFormula("unknow"),
    _iSolidColor(0),
    _dLastLambda(0.),
    _dLastIndex(-1.)
{ }
//////////////////////////////////////////////////////////////////////////////
Glass::Glass(const Glass& m):
    _sMaker(m._sMaker),
    _sName(m._sName),
    _sFormula(m._sFormula)
{
    _iSolidColor=m._iSolidColor;
    _dLastIndex=-1.;
    _dLastLambda=0.;
}
//////////////////////////////////////////////////////////////////////////////
Glass::~Glass()
{ }
//////////////////////////////////////////////////////////////////////////////
double Glass::index(double dLambdaMicrons)
{
    if(dLambdaMicrons!=_dLastLambda)
    {
        _dLastLambda=dLambdaMicrons;
        _dLastIndex= compute_index(dLambdaMicrons);
    }

    return _dLastIndex;
}
//////////////////////////////////////////////////////////////////////////////
string Glass::maker() const
{
    return _sMaker;
}
//////////////////////////////////////////////////////////////////////////////
void Glass::set_maker(const string& sMaker)
{
    _sMaker=sMaker;
}
//////////////////////////////////////////////////////////////////////////////
string Glass::name() const
{
    return _sName;
}
//////////////////////////////////////////////////////////////////////////////
void Glass::set_name(const string& sName)
{
    _sName=sName;
}
//////////////////////////////////////////////////////////////////////////////
void Glass::set_formula(const string& sFormula)
{
    _sFormula=sFormula;
}
//////////////////////////////////////////////////////////////////////////////
int Glass::solid_color() const
{
    return _iSolidColor;
}
//////////////////////////////////////////////////////////////////////////////
void Glass::set_solid_color(int iSolidColor)
{
    _iSolidColor=iSolidColor;
}
//////////////////////////////////////////////////////////////////////////////
string Glass::formula() const
{
    return _sFormula;
}
//////////////////////////////////////////////////////////////////////////////
void Glass::compute_NdVd(double &Nd,double& Vd)
{
    double dLambdaD=587.5618e-3; // He in microns
    double dLambdaF=486.13e-3; // H in microns
    double dLambdaC=656.27e-3; // H in microns

    Nd=index(dLambdaD);
    double Nf=index(dLambdaF);
    double Nc=index(dLambdaC);

    if(Nf!=Nc)
        Vd=(Nd-1.)/(Nf-Nc);
    else
        Vd=-1.; //not applicable
}
//////////////////////////////////////////////////////////////////////////////
void Glass::compute_NeVe(double &Ne, double& Ve)
{
    double dLambdae=546.07e-3; // Hg in microns
    double dLambdaFp=479.99e-3; // Cd in microns
    double dLambdaCp=643.85e-3; // Cd in microns

    Ne=index(dLambdae);
    double Nfp=index(dLambdaFp);
    double Ncp=index(dLambdaCp);

    if(Nfp!=Ncp)
        Ve=(Ne-1.)/(Nfp-Ncp);
    else
        Ve=-1.; //not applicable
}
//////////////////////////////////////////////////////////////////////////////
void Glass::compute_solid_color()
{
	double Nd, Vd;
	double t;
	compute_NdVd(Nd, Vd);

	// crown color
	double dRed90=150;
	double dGreen90=255;
	double dBlue90=255;

	//flint color
	double dRed20=30;
	double dGreen20=150;
	double dBlue20=255;

	//interpolation crown-flint
	t = (90.-Vd)/(90.-20.);
	if (t > 1.)
		t=1.;
	if (t < 0.)
		t=0.;
	double red = dRed90 * t + (1. - t)*dRed20;
	double green = dGreen90 * t + (1. - t)*dGreen20;
	double blue = dBlue90 * t + (1. - t)*dBlue20;
	_iSolidColor = (int)(((int)red<<16)+ ((int)green << 8)+ ((int)blue));
}
//////////////////////////////////////////////////////////////////////////////