// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#endif
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include "App.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/gl.h>

#include <algorithm>
#include <system_error>

namespace astree_imgui_app
{
void onWindowClose(GLFWwindow* window)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app != nullptr && app->dirty)
    {
        glfwSetWindowShouldClose(window, GLFW_FALSE);
        app->pendingAction = PendingAction::Quit;
        app->confirmPendingAction = true;
    }
    else
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onWindowContentScale(GLFWwindow* window, float xScale, float yScale)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app == nullptr)
        return;
    app->displayScale = std::max(xScale, yScale);
    if (app->useDisplayScale)
        applyTheme(*app);
}

#ifdef _WIN32
void setWindowIcon(GLFWwindow* window)
{
    HICON icon = static_cast<HICON>(LoadImageW(GetModuleHandleW(nullptr), L"ASTREE",
        IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
    if (icon == nullptr)
        return;
    const HWND nativeWindow = glfwGetWin32Window(window);
    SendMessageW(nativeWindow, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
    SendMessageW(nativeWindow, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
}
#endif
int runApp(int argc, char** argv)
{
    if (!glfwInit())
        return 1;

    App app;
    prepareAppConfig(app);
    std::string startupWarning = loadAppSettings(app);

    const char* glslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(app.windowWidth, app.windowHeight,
        "Astree - Dear ImGui", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return 1;
    }
    if (app.hasWindowPosition)
        glfwSetWindowPos(window, app.windowX, app.windowY);
#ifdef _WIN32
    setWindowIcon(window);
#endif
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
    io.IniFilename = app.imguiIniFilename.c_str();
    const std::string fontWarning = loadUiFont();
    if (!fontWarning.empty())
        startupWarning = startupWarning.empty() ? fontWarning : startupWarning + "; " + fontWarning;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    std::error_code iniError;
    app.initializeDockLayout = !fs::exists(app.imguiIniPath, iniError);
    if (iniError)
        startupWarning = "Could not check for the saved panel layout";
    glfwSetWindowUserPointer(window, &app);
    glfwSetWindowCloseCallback(window, onWindowClose);
    glfwSetWindowPosCallback(window, onWindowPosition);
    glfwSetWindowSizeCallback(window, onWindowSize);
    glfwSetWindowMaximizeCallback(window, onWindowMaximize);
    if (app.windowMaximized)
        glfwMaximizeWindow(window);
    float xScale = 1.0f;
    float yScale = 1.0f;
    glfwGetWindowContentScale(window, &xScale, &yScale);
    app.displayScale = std::max(xScale, yScale);
    applyTheme(app);
    glfwSetWindowContentScaleCallback(window, onWindowContentScale);
    loadGlassCatalogs(app);
    syncUiFromDevice(app);
    if (argc > 1)
        loadDesign(app, argv[1], false);
    if (!startupWarning.empty())
        app.status = startupWarning;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        drawUi(app);
        ImGui::Render();
        int displayWidth = 0;
        int displayHeight = 0;
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(0.10f, 0.11f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    app.windowMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
    recordWindowedGeometry(window, app);
    ImGui::SaveIniSettingsToDisk(io.IniFilename);
    saveAppSettings(app);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return astree_imgui_app::runApp(__argc, __argv);
}
#else
int main(int argc, char** argv)
{
    return astree_imgui_app::runApp(argc, argv);
}
#endif
