// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com

#include <iostream>

#include "OpticalDevice.h"

int main()
{
    int failures = 0;
    auto expect = [&](bool condition, const char* message) {
        if (!condition)
        {
            std::cerr << message << '\n';
            ++failures;
        }
    };

    OpticalDevice device;
    expect(!device.has_aspheric(), "An empty device should not be aspheric");
    expect(!device.has_inner_diameter(), "An empty device should not have an inner diameter");
    expect(!device.has_auto(), "An empty device should not have automatic parameters");
    expect(!device.has_tilt(), "An empty device should not have tilt");
    expect(!device.has_comment(), "An empty device should not have comments");
    expect(!device.is_image_infinite(), "An empty device should not have an infinite image");

    device.insert_surface(0);
    device.insert_surface(1);

    device.set(1, R4, 1e-6);
    expect(device.has_aspheric(), "A non-zero aspheric coefficient should be detected");
    device.set(1, R4, 0.0);
    expect(!device.has_aspheric(), "Zero aspheric coefficients should not be detected");

    device.set(1, INNER_DIAMETER, 2.0);
    expect(device.has_inner_diameter(), "A non-zero inner diameter should be detected");
    device.set(1, INNER_DIAMETER, 0.0);
    expect(!device.has_inner_diameter(), "A zero inner diameter should not be detected");

    device.set_autofocus(true);
    expect(device.has_auto(), "Autofocus should count as an automatic parameter");
    device.set_autofocus(false);
    device.set_image_autocurvature(true);
    expect(device.has_auto(), "Automatic image curvature should be detected");
    device.set_image_autocurvature(false);
    device.set(1, AUTO_DIAMETER, 1.0);
    expect(device.has_auto(), "Automatic diameter should be detected");
    device.set(1, AUTO_DIAMETER, 0.0);
    device.set(1, AUTO_INNER_DIAMETER, 1.0);
    expect(device.has_auto(), "Automatic inner diameter should be detected");
    device.set(1, AUTO_INNER_DIAMETER, 0.0);
    expect(!device.has_auto(), "Disabling all automatic parameters should clear the flag");

    device.set(1, TILT_X, 5.0);
    expect(device.has_tilt(), "X tilt should be detected");
    device.set(1, TILT_X, 0.0);
    device.set(1, TILT_Y, -5.0);
    expect(device.has_tilt(), "Y tilt should be detected");
    device.set(1, TILT_Y, 0.0);
    expect(!device.has_tilt(), "Zero tilt should not be detected");

    device.set_comment(1, "surface comment");
    expect(device.has_comment(), "A non-empty surface comment should be detected");
    device.set_comment(1, "");
    expect(!device.has_comment(), "Empty surface comments should not be detected");

    device.set_type(1, "image_infinite");
    expect(device.is_image_infinite(), "An infinite image surface should be detected");
    device.set_type(1, "image");
    expect(!device.is_image_infinite(), "A finite image surface should not be infinite");

    return failures == 0 ? 0 : 1;
}
