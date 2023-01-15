find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets)

set(ASTREE_GUI_SOURCES
    DialogMediumManager.cpp
    DialogRevertDesign.cpp
    DialogScaleDevice.cpp
    DockCommentary.cpp
    DockImageQuality.cpp
    DockLightProperties.cpp
    DockOptimizer.cpp
    DockScatterPlot.cpp
    DockSurfacesData.cpp
    FrameSideView.cpp
    MainWindow.cpp
    main.cpp
    DialogMediumManager.h
    DialogRevertDesign.h
    DialogScaleDevice.h
    DockCommentary.h
    DockImageQuality.h
    DockLightProperties.h
    DockOptimizer.h
    DockScatterPlot.h
    DockSurfacesData.h
    FrameSideView.h
    MainWindow.h
    MyViewFrame.h
    AstreeDefines.h
    Astree.qrc
    MainWindow.ui
    DialogRevertDesign.ui
    DockLightProperties.ui
    FrameSideView.ui
    DockScatterPlot.ui
    DockSurfacesData.ui
    DockCommentary.ui
    DialogMediumManager.ui
    DialogScaleDevice.ui
    DockImageQuality.ui
    DockOptimizer.ui
)

if(WIN32)
    list(APPEND ASTREE_GUI_SOURCES Astree.rc)
endif()

add_executable(Astree ${ASTREE_GUI_SOURCES})
target_include_directories(Astree PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Astree PRIVATE libAstree Qt6::Widgets)
target_compile_definitions(Astree PRIVATE $<$<CONFIG:Release>:QT_NO_DEBUG>)

set_target_properties(Astree PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_NAME_DEBUG Astree_debug
    OUTPUT_NAME_RELEASE Astree
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(TARGETS Astree RUNTIME DESTINATION bin)
    install(FILES "${PROJECT_SOURCE_DIR}/astree.desktop"
        DESTINATION share/applications)
    install(FILES "${PROJECT_SOURCE_DIR}/Astree.png"
        DESTINATION share/pixmaps)
    install(FILES "${PROJECT_SOURCE_DIR}/astree.appdata.xml"
        DESTINATION share/appdata)
endif()
