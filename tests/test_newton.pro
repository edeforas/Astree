TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    test_newton.cpp \
    ..\OpticalDevice.cpp \
    ..\ImageQuality.cpp \
    ..\Surface.cpp \
    ..\Photon.cpp \
    ..\Light.cpp \
    ..\LightAutofocus.cpp \
    ..\Glass.cpp \
    ..\GlassManager.cpp \
    ..\MaterialAir.cpp \
    ..\MaterialUnknow.cpp \
    ..\MaterialVacuum.cpp \
    ..\MaterialWater.cpp

HEADERS += \
    ..\OpticalDevice.h \
    ..\ImageQuality.h \
    ..\Surface.h \
    ..\Photon.h \
    ..\Light.h \
    ..\LightAutofocus.h \
    ..\Vector3D.h \
    ..\Glass.h \
    ..\GlassManager.h \
    ..\MaterialAir.h \
    ..\MaterialUnknow.h \
    ..\MaterialVacuum.h \
    ..\MaterialWater.h

INCLUDEPATH += ..
