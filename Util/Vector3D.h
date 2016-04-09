#ifndef _Vector3D_
#define _Vector3D_

#include <cmath>

namespace Vector3D
{
    inline bool is_normalized(double x,double y,double z)
    {
        double dNorm2=x*x+y*y+z*z;
        return (dNorm2<1.001) && (dNorm2>0.999);
    }

    inline void normalize(double& x,double& y,double& z)
    {
        double dNorm2=x*x+y*y+z*z;

        if(dNorm2!=0.)
        {
            dNorm2=1./std::sqrt(dNorm2);
            x*=dNorm2;
            y*=dNorm2;
            z*=dNorm2;
        }

        assert(is_normalized(x,y,z));
    }
}

/*
class Vector3D
{
public:
        Vector3D() { x=0.; y=0.; z=0.; }
        Vector3D(double dx,double dy,double dz) { x=dx; y=dy; z=dz; }

        double x,y,z;


};
*/
#endif
