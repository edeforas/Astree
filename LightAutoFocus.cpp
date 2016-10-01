// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "LightAutofocus.h"
#include "Light.h"
#include "Photon.h"

#define SPOT_SIZE_INFINITY 1.e99

////////////////////////////////////////////////////////////////////////////////
LightAutofocus::LightAutofocus()
{
    _xCenter=0.;
    _yCenter=0.;
}
////////////////////////////////////////////////////////////////////////////////
double LightAutofocus::autofocus(const Light& l)
{
    double dA=-1000.; //todo
    double dC=1000.; //todo
    double dB=(dA+dC)/2.;
    double dZStep=1.e-9;
    double dQA=compute_spot_size(l,dA);
    double dQB=compute_spot_size(l,dB);
    double dQC=compute_spot_size(l,dC);

    //1st part: expand A and C, so B became a minimum
    int iScaleLeft=0;
    while((iScaleLeft<11) && (dQA<=dQB)) //2km max
    {
        dA=dB-2*(dB-dA);
        dQA=compute_spot_size(l,dA);
        iScaleLeft++;
    }

    int iScaleRight=0;
    while((iScaleRight<11) && (dQC<=dQB)) //2km max
    {
        dC=dB-2*(dB-dC);
        dQC=compute_spot_size(l,dC);
        iScaleRight++;
    }

    //exit if no solution
    if( (dQA<=dQB) || (dQC<=dQB) )
        return 0.;

    //now reduce interval around solution
    while (dC-dA>dZStep)
    {
        double dAB=(dA+dB)/2.;
        double dQAB=compute_spot_size(l,dAB);

        if (dQAB<dQB)
        {
            // a new min
            dC=dB;
            dB=dAB;
            dQB=dQAB;
        }
        else
        {
            double dBC=(dB+dC)/2.;
            double dQBC=compute_spot_size(l,dBC);

            if (dQBC<dQB)
            {
                // a new min
                dA=dB;
                dB=dBC;
                dQB=dQBC;
            }
            else
            {
                // min is B
                dA=dAB;
                dC=dBC;
            }
        }
    }

    // the minimum is at dB;

    // todo check all photon are ok at dB, else return false; or use return value of compute_size
    // todo use golden search

    return dB;
}
//////////////////////////////////////////////////////////////////////////////
double LightAutofocus::compute_spot_size(const Light& l,double z)
{
    // compute the intersection of all valid photon with plane at (0,0,z) with normal (1,0,0)

    // todo add 45deg (or more) calliper

    // todo add early abort tests on previous spot size
    double xMax=-1.e99;
    double xMin=1.e99;

    double yMax=-1.e99;
    double yMin=1.e99;
    bool bOneFound=false;

    const vector<Photon>& photons=l.photons();

    for(unsigned int i=0;i<photons.size();i++)
    {
        const Photon& p=photons[i];
        if(!p.is_valid())
            continue;

        if(p.dz==0.)
            continue; //TODO check clean error

        //compute t at z
        double t=(z-p.z)/p.dz;

     //   if(t<=0.)
      //      return SPOT_SIZE_INFINITY; //all photons must be used in autofocus, for now

        double x=p.x+t*p.dx;
        double y=p.y+t*p.dy;

        if(!bOneFound)
        {
            xMax=xMin=x;
            yMax=yMin=y;

            bOneFound=true;
            continue;
        }

        if(x>xMax)
            xMax=x;

        if(x<xMin)
            xMin=x;

        if(y>yMax)
            yMax=y;

        if(y<yMin)
            yMin=y;
    }

    if(!bOneFound)
        return SPOT_SIZE_INFINITY;

    _xCenter=(xMax+xMin)/2.;
    _yCenter=(yMax+yMin)/2.;

    if(xMax-xMin>yMax-yMin)
        return xMax-xMin;
    else
        return yMax-yMin;
}
//////////////////////////////////////////////////////////////////////////////
void LightAutofocus::get_center(double& xCenter,double& yCenter) const
{
    xCenter=_xCenter;
    yCenter=_yCenter;
}
//////////////////////////////////////////////////////////////////////////////
