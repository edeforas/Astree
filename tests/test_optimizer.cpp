// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com

#include "DeviceOptimizerAmoeba.h"
#include "DeviceOptimizerHypercube.h"
#include "DeviceOptimizerRandom.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace
{
void test(bool condition, const std::string& message)
{
    if (condition)
        return;

    std::cerr << "Test failed: " << message << std::endl;
    std::exit(EXIT_FAILURE);
}

OpticalDevice makeSphericalMirror()
{
    OpticalDevice device;
    device.insert_surface(0);
    device.set_type(0, "reflect");
    device.set(0, DIAMETER, 200.0);
    device.set(0, RADIUS_CURVATURE, -2000.0);
    device.set(0, CONIC, 0.0);
    device.set(0, THICK, -1000.0);

    device.insert_surface(1);
    device.set_type(1, "image");
    device.set(1, AUTO_DIAMETER, 1.0);
    return device;
}

struct OptimizerCase
{
    const char* name;
    std::unique_ptr<DeviceOptimizer> (*create)();
};

std::unique_ptr<DeviceOptimizer> makeAmoeba()
{
    return std::make_unique<DeviceOptimizerAmoeba>();
}

std::unique_ptr<DeviceOptimizer> makeRandom()
{
    return std::make_unique<DeviceOptimizerRandom>();
}

std::unique_ptr<DeviceOptimizer> makeHypercube()
{
    return std::make_unique<DeviceOptimizerHypercube>();
}

const std::vector<OptimizerCase> kOptimizers = {
    { "Nelder-Mead", makeAmoeba },
    { "Monte Carlo", makeRandom },
    { "Hypercube", makeHypercube }
};

void testNoParametersReturnsNothingToOptimize()
{
    for (const OptimizerCase& optimizerCase : kOptimizers)
    {
        OpticalDevice device = makeSphericalMirror();
        const double originalConic = device.get(0, CONIC);
        std::unique_ptr<DeviceOptimizer> optimizer = optimizerCase.create();
        optimizer->set_device(&device);

        test(optimizer->optimize() == eNothingToOptimize,
            std::string(optimizerCase.name) + " should report no parameters");
        test(device.get(0, CONIC) == originalConic,
            std::string(optimizerCase.name) + " should leave the device unchanged");
    }
}

void testOptimizersImproveMirrorConic()
{
    for (const OptimizerCase& optimizerCase : kOptimizers)
    {
        std::srand(1);
        OpticalDevice device = makeSphericalMirror();
        const double initialSpotSize = device.get_image_quality().vdSpotSize[0];
        test(initialSpotSize > 0.0, "spherical mirror should have nonzero on-axis spot size");

        std::unique_ptr<DeviceOptimizer> optimizer = optimizerCase.create();
        optimizer->set_device(&device);
        optimizer->set_merit_function(eCenterOnly);
        optimizer->set_min_vignetting(0.0);
        optimizer->add_parameter(0, "Conic", -1.5, -0.5);

        const OptimizerResult result = optimizer->optimize();
        const double finalSpotSize = device.get_image_quality().vdSpotSize[0];
        test(result == eBetterSolutionFound,
            std::string(optimizerCase.name) + " should find an improved mirror");
        test(finalSpotSize < initialSpotSize,
            std::string(optimizerCase.name) + " should reduce the on-axis spot size");
        test(device.get(0, CONIC) >= -1.5 && device.get(0, CONIC) <= -0.5,
            std::string(optimizerCase.name) + " should stay within the configured bounds");
    }
}
}

int main()
{
    testNoParametersReturnsNothingToOptimize();
    testOptimizersImproveMirrorConic();
    std::cout << "all optimizer tests success!" << std::endl;
    return 0;
}
