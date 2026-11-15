// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com

#include <cmath>
#include <iostream>
#include <vector>

#include "OpticalDevice.h"

namespace
{
bool near(double actual, double expected, double tolerance = 1e-10)
{
    return std::abs(actual - expected) < tolerance;
}
}

int main()
{
    OpticalDevice device;
    device.insert_surface(0);
    device.set(0, DIAMETER, 4.0);
    device.set(0, RADIUS_CURVATURE, 10.0);
    device.set(0, Z, 2.0);

    const std::vector<SurfaceProfilePoint> fullProfile =
        device.get_surface_profile(0);
    if (fullProfile.size() != 49 ||
        !near(fullProfile[24].x, 0.0) ||
        !near(fullProfile[24].y, 0.0) ||
        !near(fullProfile[24].z, 2.0))
    {
        std::cerr << "Unexpected unmasked surface profile\n";
        return 1;
    }

    device.set(0, INNER_DIAMETER, 2.0);
    device.set(0, TILT_Y, 90.0);
    const std::vector<SurfaceProfilePoint> tiltedProfile =
        device.get_surface_profile(0);
    if (tiltedProfile.size() != 26)
    {
        std::cerr << "Inner diameter was not applied to the surface profile\n";
        return 1;
    }

    for (const SurfaceProfilePoint& point : tiltedProfile)
    {
        if (std::abs(point.y) < 1.0)
        {
            std::cerr << "Surface profile contains points inside the aperture\n";
            return 1;
        }
    }

    const SurfaceProfilePoint& innerEdge = tiltedProfile[12];
    if (!near(innerEdge.y, -1.0) ||
        !near(innerEdge.x, 0.1 / (1.0 + std::sqrt(0.99))) ||
        !near(innerEdge.z, 2.0))
    {
        std::cerr << "Surface profile tilt transform is incorrect\n";
        return 1;
    }

    return 0;
}
