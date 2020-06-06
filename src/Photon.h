// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef Photon_
#define Photon_

class Photon
{
public:
    Photon();
    bool is_valid() const;
    void set_lambda(double dLambda);
    double lambda() const;
    double x,y,z;
    double dx,dy,dz;
    double anglex,angley;

    bool valid;

private:
    double _lambda;
};

#endif
