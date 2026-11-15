// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com

#include <cmath>
#include <iostream>

#include "Light.h"
#include "Photon.h"
#include "Surface.h"

int main()
{
    Surface surface;
    surface.set_type("stop");
    surface.set_diameter(10.0);
    surface.set_tilt_y(45.0);

    Light light;
    light.set_nb_photons(3, 1);
    light.set_colors("Green.");
    light.set_geometry(-1.0, 2.0);
    Photon& photon = light.get_photon(1);
    photon.x = 1.0;
    photon.y = 0.0;
    photon.z = -2.0;
    photon.dx = 0.0;
    photon.dy = 0.0;
    photon.dz = 1.0;

    surface.receive(light);

    if (!photon.valid || !std::isfinite(photon.x) ||
        !std::isfinite(photon.y) || !std::isfinite(photon.z) ||
        std::abs(photon.x - 1.0) > 1e-10 ||
        std::abs(photon.y) > 1e-10 || std::abs(photon.z + 1.0) > 1e-10)
    {
        std::cerr << "Unexpected photon intersection: "
            << photon.x << ", " << photon.y << ", " << photon.z << "\n";
        return 1;
    }

    return 0;
}
