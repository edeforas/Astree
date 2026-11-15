include(FetchContent)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b-docking
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw imgui)

find_package(OpenGL REQUIRED)

set(ASTREE_GUI_SOURCES
    main.cpp
    AppSettings.cpp
    AppState.cpp
    FileUI.cpp
    OpticalPanels.cpp
    Panels.cpp
    LightPanel.cpp
    NotesPanel.cpp
    OptimizerPanel.cpp
    MediumManagerPanel.cpp
    CloneEditorPanel.cpp
    ScaleDialog.cpp
    RevertDialog.cpp
    AboutDialog.cpp
    PendingActionDialog.cpp
    ResetSettingsDialog.cpp
    StatusBar.cpp
)

add_library(astree_imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(astree_imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(astree_imgui PUBLIC glfw OpenGL::GL)

if(WIN32)
    enable_language(RC)
    add_executable(Astree WIN32 ${ASTREE_GUI_SOURCES})
    target_sources(Astree PRIVATE "${PROJECT_SOURCE_DIR}/gui/Astree.rc")
else()
    add_executable(Astree ${ASTREE_GUI_SOURCES})
endif()
target_include_directories(Astree PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(Astree PRIVATE libAstree astree_imgui)
if(WIN32)
    target_link_libraries(Astree PRIVATE Dwmapi Ole32 Shell32)
endif()
target_compile_definitions(Astree PRIVATE
    ASTREE_GLASS_DIR="${PROJECT_SOURCE_DIR}/glass"
)

if(MSVC)
    target_compile_options(Astree PRIVATE /W4)
else()
    target_compile_options(Astree PRIVATE -Wall -Wextra -Wpedantic)
endif()
