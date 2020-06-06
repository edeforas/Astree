// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <iostream>
#include <cmath>
using namespace std;

#include "OpticalDevice.h"
#include "DeviceScaling.h"

/////////////////////////////////////////////////////////////////////
inline bool is_near(double a,double b, double tolerancis=1e-10)
{
    return fabs(a-b)<tolerancis;
}

void test(bool bTest,const string& sMessage)
{
    if(bTest) return;

    cout << "Test failed: " << sMessage << endl;
    exit(-1);
}
/////////////////////////////////////////////////////////////////////

void test_scaling_thick()
{
    OpticalDevice od;

    // surface0 = perfect lens
    od.insert_surface(0);
    od.set_type(0,"perfect_lens");
    od.set(0,DIAMETER,200);
    od.set(0,RADIUS_CURVATURE,2000); // F=-1000mm
    od.set(0,THICK,1000);

    test(is_near(200.,od.get(0,DIAMETER)),"D=200mm");
    test(is_near(2000.,od.get(0,RADIUS_CURVATURE)),"RC=2000mm");
    test(is_near(1000.,od.get(0,THICK)),"THICK=1000mm");

    DeviceScaling d;
    d.scale(&od,2.);

    test(is_near(2.*200.,od.get(0,DIAMETER)),"D=400mm");
    test(is_near(2.*2000.,od.get(0,RADIUS_CURVATURE)),"RC=4000mm");
    test(is_near(2.*1000.,od.get(0,THICK)),"THICK=2000mm");
}

/////////////////////////////////////////////////////////////////////

void test_scaling_Z()
{
    OpticalDevice od;

    // surface0 = perfect lens
    od.insert_surface(0);
    od.set_type(0,"perfect_lens");
    od.set(0,DIAMETER,200);
    od.set(0,RADIUS_CURVATURE,2000); // F=-1000mm
    od.set(0,Z,1000);

    test(is_near(200.,od.get(0,DIAMETER)),"D=200mm");
    test(is_near(2000.,od.get(0,RADIUS_CURVATURE)),"RC=2000mm");
    test(is_near(1000.,od.get(0,Z)),"Z=1000mm");

    DeviceScaling d;
    d.scale(&od,2.);

    test(is_near(2.*200.,od.get(0,DIAMETER)),"D=400mm");
    test(is_near(2.*2000.,od.get(0,RADIUS_CURVATURE)),"RC=4000mm");
    test(is_near(2.*1000.,od.get(0,Z)),"Z=2000mm");
}

/////////////////////////////////////////////////////////////////////

int main()
{
    test_scaling_thick();
    test_scaling_Z();

    cout << "all tests success!" << endl;

    return 0;
}
