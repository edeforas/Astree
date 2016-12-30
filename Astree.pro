TEMPLATE = app
DESTDIR = ../Astree
QT += widgets

CONFIG(debug, debug|release):TARGET = Astree_debug
CONFIG(release, debug|release):TARGET = Astree
CONFIG(release, debug|release):DEFINES+=NDEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG

#CONFIG(debug, debug|release):QMAKE_CXXFLAGS_DEBUG+= -pg
#CONFIG(debug, debug|release):QMAKE_LFLAGS_DEBUG+= -pg

win32:RC_FILE = Astree.rc
RESOURCES += Astree.qrc
TRANSLATIONS = Astree_fr.ts
SOURCES += main.cpp \
    MainWindow.cpp \
    DockLightProperties.cpp \
    FrameSideView.cpp \
    GlassCatalogIo.cpp \
    DockScatterPlot.cpp \
    DockSurfacesData.cpp \
    DockCommentary.cpp \
    MaterialVacuum.cpp \
    DialogMediumManager.cpp \
    OpticalDevice.cpp \
    DialogScaleDevice.cpp \
    DeviceIoZmx.cpp \
    LightAutoFocus.cpp \
    DeviceScaling.cpp \
    DockImageQuality.cpp \
    ImageQuality.cpp \
    DockOptimizer.cpp \
    DeviceOptimizer.cpp \
    DeviceOptimizerAmoeba.cpp \
    DeviceOptimizerRandom.cpp \
    Util/FileUtil.cpp \
    Glass.cpp \
    GlassManager.cpp \
    GlassSellmeier.cpp
	
HEADERS += MainWindow.h \
    DockLightProperties.h \
    FrameSideView.h \
    DockScatterPlot.h \
    DockSurfacesData.h \
    DockCommentary.h \
    MyViewFrame.h \
    Util/FileUtil.h \
    MaterialVacuum.h \
    DialogMediumManager.h \
    OpticalDevice.h \
    DialogScaleDevice.h \
    DeviceIoZmx.h \
    LightAutoFocus.h \
    DeviceScaling.h \
    DockImageQuality.h \
    ImageQuality.h \
    DockOptimizer.h \
    DeviceOptimizer.h \
    DeviceOptimizerAmoeba.h \
    DeviceOptimizerRandom.h \
    GlassManager.h \
    GlassCatalogIo.h \
    Glass.h \
    GlassSellmeier.h
	
FORMS += MainWindow.ui \
    DockLightProperties.ui \
    FrameSideView.ui \
    DockScatterPlot.ui \
    DockSurfacesData.ui \
    DockCommentary.ui \
    DialogMediumManager.ui \
    DialogScaleDevice.ui \
    DockImageQuality.ui \
    DockOptimizer.ui

# internal engine files
INCLUDEPATH += Util
SOURCES += \
    DeviceIo.cpp \
    Surface.cpp \
    Photon.cpp \
    Referential.cpp \
    Light.cpp \
    Photon.h \
    MaterialUnknow.cpp \
    MaterialAir.cpp \
    MaterialWater.cpp \
    Util/Properties.cpp

HEADERS += \
    DeviceIo.h \
    Surface.h \
    Referential.h \
    Light.h \
    Photon.h \
    MaterialAir.h \
    MaterialWater.h \
    MaterialUnknow.h \
    Util/Properties.h \
    Util/Vector3D.h \
    AstreeDefines.h
	
OTHER_FILES += Astree.iss
