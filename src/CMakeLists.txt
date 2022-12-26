set(ASTREE_FILES
	OpticalDevice.h OpticalDevice.cpp
	ImageQuality.h ImageQuality.cpp
	Surface.h Surface.cpp
	Light.h Light.cpp
	LightAutofocus.h LightAutofocus.cpp
	Photon.h Photon.cpp
	Glass.h Glass.cpp
	GlassManager.h GlassManager.cpp
	MaterialAir.h MaterialAir.cpp
	MaterialVacuum.h MaterialVacuum.cpp
	MaterialUnknow.h MaterialUnknow.cpp
	MaterialWater.h MaterialWater.cpp
	DeviceScaling.h DeviceScaling.cpp 
)
include_directories(.)
add_library( libAstree STATIC ${ASTREE_FILES})
