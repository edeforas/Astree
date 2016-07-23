// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
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
        _dLastIndex=calc_index(dLambdaMicrons);
    }

    return _dLastIndex;
}
//////////////////////////////////////////////////////////////////////////////
string Glass::maker() const
{
    return _sMaker;
}
//////////////////////////////////////////////////////////////////////////////
void Glass::set_maker(string sMaker)
{
    _sMaker=sMaker;
}
//////////////////////////////////////////////////////////////////////////////
string Glass::name() const
{
    return _sName;
}
//////////////////////////////////////////////////////////////////////////////
void Glass::set_name(string sName)
{
    _sName=sName;
}
//////////////////////////////////////////////////////////////////////////////
void Glass::set_formula(string sFormula)
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
void Glass::compute_abbe_number(double &Nd,double& Vd)
{
    double dLambdaD=583.9*1e-3; //sodium in microns
    double dLambdaF=486.1*1e-3; //hydrogen1 in microns
    double dLambdaC=656.3*1e-3; //hydrogen2 in microns

    Nd=index(dLambdaD);
    double Nf=index(dLambdaF);
    double Nc=index(dLambdaC);

    if(Nf!=Nc)
        Vd=(Nd-1.)/(Nf-Nc);
    else
        Vd=-1.; //not applicable
}
//////////////////////////////////////////////////////////////////////////////
