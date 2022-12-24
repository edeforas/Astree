cmake_minimum_required(VERSION 3.0.0)
project (tests_astree)

set(ASTREE_FILES
	../OpticalDevice.h ../OpticalDevice.cpp
	../ImageQuality.h ../ImageQuality.cpp
	../Surface.h ../Surface.cpp
	../Light.h ../Light.cpp
	../LightAutofocus.h ../LightAutofocus.cpp
	../Photon.h ../Photon.cpp
	../Glass.h ../Glass.cpp
	../GlassManager.h ../GlassManager.cpp
	../MaterialAir.h ../MaterialAir.cpp
	../MaterialVacuum.h ../MaterialVacuum.cpp
	../MaterialUnknow.h ../MaterialUnknow.cpp
	../MaterialWater.h ../MaterialWater.cpp
	../DeviceScaling.h ../DeviceScaling.cpp 
)
include_directories(..)
add_library( libAstree STATIC ${ASTREE_FILES})

add_executable(test_newton test_newton.cpp  )
target_link_libraries(test_newton libAstree)

add_executable(test_perfect_surface test_perfect_surface.cpp  )
target_link_libraries(test_perfect_surface libAstree)

add_executable(test_scaling test_scaling.cpp  )
target_link_libraries(test_scaling libAstree)