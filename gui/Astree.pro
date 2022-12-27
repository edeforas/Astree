TEMPLATE = app
QT  += core gui widgets
DESTDIR= ../out

CONFIG+=c++17

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
    DialogRevertDesign.cpp \
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
    DialogRevertDesign.h \
    DockImageQuality.h \
    DockOptimizer.h \
    AstreeDefines.h

FORMS += MainWindow.ui \
    DialogRevertDesign.ui \
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
INCLUDEPATH += ../src
SOURCES += \
    ../src/DeviceIo.cpp \
    ../src/Surface.cpp \
    ../src/Photon.cpp \
    ../src/Referential.cpp \
    ../src/Light.cpp \
    ../src/Photon.h \
    ../src/MaterialUnknow.cpp \
    ../src/MaterialAir.cpp \
    ../src/MaterialWater.cpp \
    ../src/OpticalDevice.cpp \
    ../src/ImageQuality.cpp \
    ../src/GlassCatalogIo.cpp \
    ../src/MaterialVacuum.cpp \
    ../src/DeviceIoZmx.cpp \
    ../src/LightAutoFocus.cpp \
    ../src/DeviceScaling.cpp \
    ../src/DeviceRevert.cpp \
    ../src/DeviceOptimizer.cpp \
    ../src/DeviceOptimizerAmoeba.cpp \
    ../src/DeviceOptimizerRandom.cpp \
    ../src/DeviceOptimizerHypercube.cpp \
    ../src/FileUtil.cpp \
    ../src/Glass.cpp \
    ../src/GlassManager.cpp \
    ../src/GlassSellmeier.cpp \
    ../src/GlassExtended2.cpp \
    ../src/Properties.cpp

HEADERS += \
    ../src/DeviceIo.h \
    ../src/Surface.h \
    ../src/Referential.h \
    ../src/Light.h \
    ../src/Photon.h \
    ../src/MaterialAir.h \
    ../src/MaterialWater.h \
    ../src/MaterialUnknow.h \
    ../src/ImageQuality.h \
    ../src/OpticalDevice.h \
    ../src/DeviceIoZmx.h \
    ../src/LightAutoFocus.h \
    ../src/DeviceScaling.h \
    ../src/DeviceRevert.h \
    ../src/DeviceOptimizer.h \
    ../src/DeviceOptimizerAmoeba.h \
    ../src/DeviceOptimizerRandom.h \
    ../src/DeviceOptimizerHypercube.h \
    ../src/FileUtil.h \
    ../src/MaterialVacuum.h \
    ../src/GlassManager.h \
    ../src/GlassCatalogIo.h \
    ../src/Glass.h \
    ../src/GlassSellmeier.h \
    ../src/GlassExtended2.h \
    ../src/Vector3D.h \
    ../src/Properties.h

DISTFILES += \
    Astree.ico
