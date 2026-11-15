set(ASTREE_FILES
    DeviceIo.h DeviceIo.cpp
    DeviceIoZmx.h DeviceIoZmx.cpp
    DeviceOptimizer.h DeviceOptimizer.cpp
    DeviceOptimizerAmoeba.h DeviceOptimizerAmoeba.cpp
    DeviceOptimizerHypercube.h DeviceOptimizerHypercube.cpp
    DeviceOptimizerRandom.h DeviceOptimizerRandom.cpp
    DeviceRevert.h DeviceRevert.cpp
    DeviceScaling.h DeviceScaling.cpp
    FileUtil.h FileUtil.cpp
    Glass.h Glass.cpp
    GlassCatalogIo.h GlassCatalogIo.cpp
    GlassExtended2.h GlassExtended2.cpp
    GlassManager.h GlassManager.cpp
    GlassSellmeier.h GlassSellmeier.cpp
    ImageQuality.h ImageQuality.cpp
    Light.h Light.cpp
    LightAutoFocus.cpp LightAutofocus.h
    MaterialAir.h MaterialAir.cpp
    MaterialUnknow.h MaterialUnknow.cpp
    MaterialVacuum.h MaterialVacuum.cpp
    MaterialWater.h MaterialWater.cpp
    OpticalDevice.h OpticalDevice.cpp
    Photon.h Photon.cpp
    Properties.h Properties.cpp
    Referential.h Referential.cpp
    Surface.h Surface.cpp
    Vector3D.h
)

add_library(libAstree STATIC ${ASTREE_FILES})
target_include_directories(libAstree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
