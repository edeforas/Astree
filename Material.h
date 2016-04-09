// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _Material_
#define _Material_

#include <string>
using namespace std;

class Material
{
public:
    Material();
    Material(const Material& m);

    virtual Material* clone() const =0;
    virtual ~Material();

    double index(double dLambdaMicrons);
    int solid_color() const;
    void set_solid_color(int iSolidColor);

    string name() const;
    void set_name(string sName);

    string formulae() const;
    void set_formulae(string sFormulae);

    void compute_abbe_number(double &Nd,double& Vd);
protected:
    virtual double calc_index(double dLambdaMicrons)=0;

    string _sName;
    string _sFormulae;
    int _iSolidColor;

private:
    double _dLastLambda;
    double _dLastIndex;
};

#endif
