TEMPLATE = app
QT += widgets
DESTDIR= ..

CONFIG(debug, debug|release):TARGET = Astree_debug
CONFIG(release, debug|release):TARGET = Astree
CONFIG(release, debug|release):DEFINES+=NDEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG

win32:RC_FILE = Astree.rc
RESOURCES += Astree.qrc
TRANSLATIONS = Astree_fr.ts
SOURCES += main.cpp \
    MainWindow.cpp \
    DockLightProperties.cpp \
    FrameSideView.cpp \
    DockScatterPlot.cpp \
    DockSurfacesData.cpp \
    DockCommentary.cpp \
    DialogMediumManager.cpp \
    DialogScaleDevice.cpp \
    DockImageQuality.cpp \
    DockOptimizer.cpp
	
HEADERS += MainWindow.h \
    DockLightProperties.h \
    FrameSideView.h \
    DockScatterPlot.h \
    DockSurfacesData.h \
    DockCommentary.h \
    MyViewFrame.h \
    DialogMediumManager.h \
    DialogScaleDevice.h \
    DockImageQuality.h \
    DockOptimizer.h \
    AstreeDefines.h

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
INCLUDEPATH += ..
SOURCES += \
    ../DeviceIo.cpp \
    ../Surface.cpp \
    ../Photon.cpp \
    ../Referential.cpp \
    ../Light.cpp \
    ../Photon.h \
    ../MaterialUnknow.cpp \
    ../MaterialAir.cpp \
    ../MaterialWater.cpp \
    ../OpticalDevice.cpp \
    ../ImageQuality.cpp \
    ../GlassCatalogIo.cpp \
    ../MaterialVacuum.cpp \
    ../DeviceIoZmx.cpp \
    ../LightAutoFocus.cpp \
    ../DeviceScaling.cpp \
    ../DeviceOptimizer.cpp \
    ../DeviceOptimizerAmoeba.cpp \
    ../DeviceOptimizerRandom.cpp \
    ../FileUtil.cpp \
    ../Glass.cpp \
    ../GlassManager.cpp \
    ../GlassSellmeier.cpp \
    ../GlassExtended2.cpp \
    ../Properties.cpp

HEADERS += \
    ../DeviceIo.h \
    ../Surface.h \
    ../Referential.h \
    ../Light.h \
    ../Photon.h \
    ../MaterialAir.h \
    ../MaterialWater.h \
    ../MaterialUnknow.h \
    ../ImageQuality.h \
    ../OpticalDevice.h \
    ../DeviceIoZmx.h \
    ../LightAutoFocus.h \
    ../DeviceScaling.h \
    ../DeviceOptimizer.h \
    ../DeviceOptimizerAmoeba.h \
    ../DeviceOptimizerRandom.h \
    ../FileUtil.h \
    ../MaterialVacuum.h \
    ../GlassManager.h \
    ../GlassCatalogIo.h \
    ../Glass.h \
    ../GlassSellmeier.h \
    ../GlassExtended2.h \
    ../Vector3D.h \
    ../Properties.h
	
OTHER_FILES += Astree.iss
