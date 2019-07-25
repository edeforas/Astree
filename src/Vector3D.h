#ifndef Vector3D_
#define Vector3D_

#include <cmath>

namespace Vector3D
{
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
    }
}

#endif
