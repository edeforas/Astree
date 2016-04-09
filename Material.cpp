// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "Material.h"

//////////////////////////////////////////////////////////////////////////////
Material::Material():
    _sName("unknow"),
    _sFormulae("unknow"),
    _iSolidColor(0),
    _dLastLambda(0.),
    _dLastIndex(-1.)
{ }
//////////////////////////////////////////////////////////////////////////////
Material::Material(const Material& m):
    _sName(m._sName),
    _sFormulae(m._sFormulae)
{
    _iSolidColor=m._iSolidColor;
    _dLastIndex=-1.;
    _dLastLambda=0.;
}
//////////////////////////////////////////////////////////////////////////////
Material::~Material()
{ }
//////////////////////////////////////////////////////////////////////////////
double Material::index(double dLambdaMicrons)
{
    if(dLambdaMicrons!=_dLastLambda)
    {
        _dLastLambda=dLambdaMicrons;
        _dLastIndex=calc_index(dLambdaMicrons);
    }

    return _dLastIndex;
}
//////////////////////////////////////////////////////////////////////////////
string Material::name() const
{
    return _sName;
}
//////////////////////////////////////////////////////////////////////////////
void Material::set_name(string sName)
{
    _sName=sName;
}
//////////////////////////////////////////////////////////////////////////////
void Material::set_formulae(string sFormulae)
{
    _sFormulae=sFormulae;
}
//////////////////////////////////////////////////////////////////////////////
int Material::solid_color() const
{
    return _iSolidColor;
}
//////////////////////////////////////////////////////////////////////////////
void Material::set_solid_color(int iSolidColor)
{
    _iSolidColor=iSolidColor;
}
//////////////////////////////////////////////////////////////////////////////
string Material::formulae() const
{
    return _sFormulae;
}
//////////////////////////////////////////////////////////////////////////////
void Material::compute_abbe_number(double &Nd,double& Vd)
{
    double dLambdaD=583.9*1e-3; //sodium
    double dLambdaF=486.1*1e-3; //hydrogen1
    double dLambdaC=656.3*1e-3; //hydrogen2

    Nd=index(dLambdaD);
    double Nf=index(dLambdaF);
    double Nc=index(dLambdaC);

    if(Nf!=Nc)
        Vd=(Nd-1.)/(Nf-Nc);
    else
        Vd=-1.; //not applicable
}
//////////////////////////////////////////////////////////////////////////////
