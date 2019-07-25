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
void test_perfect_mirror()
{
    OpticalDevice od;

    // surface0 = perfect mirror
    od.insert_surface(0);
    od.set_type(0,"perfect_mirror");
    od.set(0,DIAMETER,200);
    od.set(0,RADIUS_CURVATURE,-2000); // F=-1000mm
    od.set(0,THICK,-1000);

    // surface1 = image
    od.insert_surface(1);
    od.set_type(1,"image");
    od.set(1,AUTO_DIAMETER,1);

    ImageQuality iq=od.get_image_quality();

    test(is_near(5,iq.dFNumber),"FNumber must be equal");
    test(is_near(0,iq.vdSpotSize[0]),"Spot size must be zero");
    test(is_near(0,iq.vdSpotvsAiry[0]),"Spot size vs Airy must be zero");
    test(is_near(100,iq.vdVignetting[0]),"Vignetting must be 100%");
}
/////////////////////////////////////////////////////////////////////
void test_perfect_lens()
{
    OpticalDevice od;

    // surface0 = perfect lens
    od.insert_surface(0);
    od.set_type(0,"perfect_lens");
    od.set(0,DIAMETER,200);
    od.set(0,RADIUS_CURVATURE,2000); // F=-1000mm
    od.set(0,THICK,1000);

    // surface1 = image
    od.insert_surface(1);
    od.set_type(1,"image");

    ImageQuality iq=od.get_image_quality();

    test(is_near(5,iq.dFNumber),"FNumber must be equal");
    test(is_near(0,iq.vdSpotSize[0]),"Spot size must be zero");
    test(is_near(0,iq.vdSpotvsAiry[0]),"Spot size vs Airy must be zero");
    test(is_near(100,iq.vdVignetting[0]),"Vignetting must be 100%");
}
/////////////////////////////////////////////////////////////////////
int main()
{
    test_perfect_mirror();
    test_perfect_lens();

    cout << "all tests success!" << endl;

    return 0;
}
