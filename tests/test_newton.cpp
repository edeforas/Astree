// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <iostream>
#include <cmath>
#include <chrono>
using namespace std;

#include "OpticalDevice.h"

/////////////////////////////////////////////////////////////////////
inline bool is_near(double a,double b, double tolerancis=1.e-3)
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
	cout << "Test parabolic mirror start..." << endl;
	OpticalDevice od;
    double dDiameter=200.;
    double dFocal=1000.;

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
    od.set(1,AUTO_DIAMETER,1);

    ImageQuality iq=od.get_image_quality();

    //for fast parabolic mirror ; FNumber formula is :
    double fNumber=dFocal/dDiameter-dDiameter/dFocal/16.;

    test(is_near(fNumber,iq.dFNumber),"FNumber must be equal");
    test(is_near(0,iq.vdSpotSize[0]),"Spot size must be zero");
    test(is_near(0,iq.vdSpotvsAiry[0]),"Spot size vs Airy must be zero");
    test(is_near(100,iq.vdVignetting[0]),"Vignetting must be 100%");
	cout <<"Test parabolic mirror ok." << endl;
}
/////////////////////////////////////////////////////////////////////
void test_tilted_image()
{
    cout << "Test tilted image start..." << endl;
    OpticalDevice od;
    od.insert_surface(0);
    od.set_type(0,"reflect");
    od.set(0,DIAMETER,200.);
    od.set(0,RADIUS_CURVATURE,-2000.);
    od.set(0,CONIC,-1.);
    od.set(0,THICK,-1000.);

    od.insert_surface(1);
    od.set_type(1,"image");
    od.set(1,AUTO_DIAMETER,1);
    od.set(1,TILT_X,20.);
    od.set_nb_intermediate_angles(3);
    od.set_half_field_of_view(0.38);

    test(is_near(20.,od.get(1,TILT_X,false)),"Tilt X must be stored");
    test(is_near(0.,od.get(1,TILT_Y,false)),"Tilt Y must default to zero");

    ImageQuality iq=od.get_image_quality();
    test(iq.nb_angles()==5,"Tilted image quality must include negative and positive angles");
    test(is_near(iq.vdAngles[0],-od.half_field_of_view()),"Negative field angle must be included");
    test(is_near(iq.vdAngles[2],0.),"Axis angle must remain present");
    test(is_near(iq.vdAngles[4],od.half_field_of_view()),"Positive field angle must be included");
    test(iq.vdDist[0] < 0. && iq.vdDist[4] > 0.,
        "Negative and positive tilted fields must produce opposite image coordinates");
    test(iq.vdSpotSize[0]>0.,"A 20 degree tilted image must not remain diffraction-perfect");
    cout << "Test tilted image ok." << endl;
}
/////////////////////////////////////////////////////////////////////
void test_computation_time()
{
	cout << "Test Duration start..." << endl;
	int iNbTests = 100;
	chrono::steady_clock::time_point T1 = chrono::steady_clock::now();

	for (int iTest = 0; iTest < iNbTests; iTest++)
	{
		OpticalDevice od;
		double dDiameter = 200.;
		double dFocal = 1000.;

		// surface0 = parabolic mirror
		od.insert_surface(0);
		od.set_type(0, "reflect");
		od.set(0, DIAMETER, dDiameter);
		od.set(0, RADIUS_CURVATURE, -2.*dFocal);
		od.set(0, CONIC, -1.);
		od.set(0, THICK, -dFocal);

		// surface1 = image
		od.insert_surface(1);
		od.set_type(1, "image");
		od.set(1, AUTO_DIAMETER, 1);
		
		ImageQuality iq = od.get_image_quality();

		//for fast parabolic mirror ; FNumber formula is :
		double fNumber = dFocal / dDiameter - dDiameter / dFocal / 16.;

		test(is_near(fNumber, iq.dFNumber), "FNumber must be equal");
		test(is_near(0, iq.vdSpotSize[0]), "Spot size must be zero");
		test(is_near(0, iq.vdSpotvsAiry[0]), "Spot size vs Airy must be zero");
		test(is_near(100, iq.vdVignetting[0]), "Vignetting must be 100%");
	}

	chrono::steady_clock::time_point T2 = chrono::steady_clock::now();
	auto delta = chrono::duration_cast<std::chrono::milliseconds>(T2-T1).count();
	cout << "Duration: " << delta/(double)iNbTests << " ms" << endl;
	cout << "Test Duration ok." << endl;
}
/////////////////////////////////////////////////////////////////////
int main()
{
    test_parabolic_mirror();
    test_tilted_image();
	test_computation_time();

    cout << "All tests ok." << endl;

    return 0;
}
