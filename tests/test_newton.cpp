#include <iostream>
#include <cmath>
using namespace std;

#include "OpticalDevice.h"

/////////////////////////////////////////////////////////////////////
inline bool is_near(double a,double b, double tolerancis=1e-10)
{
    return fabs(a-b)<tolerancis;
}

void test(bool bTest,string sMessage)
{
    if(bTest) return;

    cout << "Test failed: " << sMessage << endl;
    exit(-1);
}
/////////////////////////////////////////////////////////////////////
void test_parabolic_mirror()
{
    OpticalDevice od;
    double dDiameter=200;
    double dFocal=1000;

    // surface0 = parabolic mirror
    od.insert_surface(0);
    od.set_type(0,"reflect");
    od.set(0,DIAMETER,dDiameter);
    od.set(0,RADIUS_CURVATURE,-2.*dFocal);
    od.set(0,CONIC,-1.);
    od.set(0,THICK,-dFocal);

    // surface1 = image
    od.insert_surface(1);
    od.set_type(1,"image");

    ImageQuality iq=od.get_image_quality();

    //for fast parabolic miror ; FNumber formula is
    double fNumber=dFocal/dDiameter-dDiameter/dFocal/16.;

    test(is_near(fNumber,iq.dFNumber),"FNumber must be equal");
    test(is_near(0,iq.vdSpotSize[0]),"Spot size must be zero");
    test(is_near(0,iq.vdSpotvsAiry[0]),"Spot size vs Airy must be zero");
    test(is_near(100,iq.vdVignetting[0]),"Vignetting must be 100%");
}
/////////////////////////////////////////////////////////////////////
int main()
{
    test_parabolic_mirror();

    cout << "all tests success!" << endl;

    return 0;
}
