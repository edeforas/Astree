// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef Glass_
#define Glass_

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
    void set_maker(const string& sMaker);

    string name() const;
    void set_name(const string& sName);

    string formula() const;
    void set_formula(const string& sFormula);

    void compute_NdVd(double &Nd,double& Vd);
    void compute_NeVe(double &Ne,double& Ve);

protected:
    virtual double compute_index(double dLambdaMicrons)=0;
	void compute_solid_color();

    string _sMaker;
    string _sName;
    string _sFormula;
    int _iSolidColor;

private:
    double _dLastLambda;
    double _dLastIndex;
};

#endif
