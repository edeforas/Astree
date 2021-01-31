# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef Glass_
#define Glass_

#include <string>
using namespace std

class Glass
public:
    Glass()
    Glass( Glass& m)

    virtual Glass* clone()  =0
    virtual ~Glass()

    double index(double dLambdaMicrons)
    int solid_color()
    void set_solid_color(int iSolidColor)

    string maker()
    void set_maker( string& sMaker)

    string name()
    void set_name( string& sName)

    string formula()
    void set_formula( string& sFormula)

    void compute_NdVd(double &Nd, Vd)
    void compute_NeVe(double &Ne, Ve)

protected:
    virtual double compute_index(double dLambdaMicrons)=0
	void compute_solid_color()

    string _sMaker
    string _sName
    string _sFormula
    int _iSolidColor

private:
    double _dLastLambda
    double _dLastIndex


#endif
