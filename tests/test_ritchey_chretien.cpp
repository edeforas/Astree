// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com

#include "OpticalDevice.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace
{
void test(bool condition, const char* message)
{
    if (condition)
        return;

    std::cerr << "Test failed: " << message << std::endl;
    std::exit(EXIT_FAILURE);
}

OpticalDevice makeRitcheyChretien()
{
    OpticalDevice device;

    device.insert_surface(0);
    device.set_type(0, "reflect");
    device.set(0, DIAMETER, 400.0);
    device.set(0, INNER_DIAMETER, 60.0);
    device.set(0, RADIUS_CURVATURE, -2406.015038);
    device.set(0, CONIC, -1.136753441);
    device.set(0, THICK, -809.3233083);

    device.insert_surface(1);
    device.set_type(1, "reflect");
    device.set(1, DIAMETER, 139.357288);
    device.set(1, AUTO_DIAMETER, 1.0);
    device.set(1, RADIUS_CURVATURE, -1259.849624);
    device.set(1, CONIC, -6.499526963);
    device.set(1, THICK, 1049.837418);

    device.insert_surface(2);
    device.set_type(2, "image");
    device.set(2, DIAMETER, 31.36525282);
    device.set(2, AUTO_DIAMETER, 1.0);
    device.set_autofocus(true);
    device.set_image_autocurvature(true);
    device.set_half_field_of_view(0.28);

    return device;
}

void testRitcheyChretienOnAxisImage()
{
    OpticalDevice device = makeRitcheyChretien();
    const ImageQuality quality = device.get_image_quality();

    test(std::isfinite(quality.dFNumber), "F-number should be finite");
    test(std::abs(quality.dFNumber - 8.0) < 0.05, "F-number should be about f/8");
    test(quality.vdSpotSize[0] < 0.01, "on-axis spot should be small");
    test(quality.vdVignetting[0] > 99.0, "on-axis pupil should not be vignetted");
}
}

int main()
{
    testRitcheyChretienOnAxisImage();
    std::cout << "all Ritchey-Chretien tests success!" << std::endl;
    return 0;
}
