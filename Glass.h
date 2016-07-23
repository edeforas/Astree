// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef _Material_
#define _Material_

#include <string>
using namespace std;

class Glass
{
public:
    Glass();
    Glass(const Glass& m);

    virtual Glass* clone() const =0;
    virtual ~Glass();

    double index(double dLambdaMicrons);
    int solid_color() const;
    void set_solid_color(int iSolidColor);

    string maker() const;
    void set_maker(string sMaker);

    string name() const;
    void set_name(string sName);

    string formula() const;
    void set_formula(string sFormula);

    void compute_abbe_number(double &Nd,double& Vd);
protected:
    virtual double calc_index(double dLambdaMicrons)=0;

    string _sMaker;
    string _sName;
    string _sFormula;
    int _iSolidColor;

private:
    double _dLastLambda;
    double _dLastIndex;
};

#endif
