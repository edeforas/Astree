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
#include <dwmapi.h>
#endif

#include "App.h"
#include "imgui_internal.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace astree_imgui_app
{
#ifdef _WIN32
void updateNativeTitleBar(UiTheme theme)
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (window == nullptr)
        return;

    const BOOL useDarkTitleBar = theme != UiTheme::Light;
    const HWND nativeWindow = glfwGetWin32Window(window);
    HRESULT result = DwmSetWindowAttribute(nativeWindow,
        static_cast<DWMWINDOWATTRIBUTE>(20), &useDarkTitleBar, sizeof(useDarkTitleBar));
    if (FAILED(result))
        DwmSetWindowAttribute(nativeWindow, static_cast<DWMWINDOWATTRIBUTE>(19),
            &useDarkTitleBar, sizeof(useDarkTitleBar));
}
#endif

void applyTheme(const App& app)
{
    ImGui::GetStyle() = ImGuiStyle();
    switch (app.theme)
    {
    case UiTheme::Dark:
        ImGui::StyleColorsDark();
        {
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowPadding = ImVec2(12.0f, 10.0f);
            style.FramePadding = ImVec2(8.0f, 5.0f);
            style.ItemSpacing = ImVec2(8.0f, 6.0f);
            style.CellPadding = ImVec2(6.0f, 5.0f);
            style.WindowRounding = 6.0f;
            style.FrameRounding = 5.0f;
            style.PopupRounding = 6.0f;
            style.ScrollbarRounding = 6.0f;
            style.GrabRounding = 5.0f;
            style.TabRounding = 5.0f;
            style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.92f, 0.95f, 1.00f);
            style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.47f, 0.52f, 0.60f, 1.00f);
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
            style.Colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.17f, 0.20f, 1.00f);
            style.Colors[ImGuiCol_Border] = ImVec4(0.23f, 0.25f, 0.29f, 1.00f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.18f, 0.21f, 1.00f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.23f, 0.28f, 1.00f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.26f, 0.33f, 1.00f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.22f, 0.26f, 0.31f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.29f, 0.35f, 0.43f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.44f, 0.54f, 1.00f);
            style.Colors[ImGuiCol_CheckMark] = ImVec4(0.14f, 0.57f, 0.96f, 1.00f);
            style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.16f, 0.54f, 0.89f, 1.00f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.63f, 1.00f, 1.00f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.17f, 0.23f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.31f, 0.42f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.36f, 0.56f, 1.00f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.17f, 0.23f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.22f, 0.31f, 0.42f, 1.00f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.36f, 0.56f, 1.00f);
            style.Colors[ImGuiCol_Separator] = ImVec4(0.23f, 0.25f, 0.29f, 1.00f);
            style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.16f, 0.54f, 0.89f, 1.00f);
            style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.24f, 0.63f, 1.00f, 1.00f);
            style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.16f, 0.54f, 0.89f, 0.20f);
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.16f, 0.54f, 0.89f, 0.67f);
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.63f, 1.00f, 0.95f);
            style.Colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.22f, 0.29f, 0.38f, 1.00f);
            style.Colors[ImGuiCol_TabSelected] = ImVec4(0.17f, 0.20f, 0.25f, 1.00f);
            style.Colors[ImGuiCol_TabDimmed] = ImVec4(0.12f, 0.14f, 0.17f, 1.00f);
            style.Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.15f, 0.17f, 0.20f, 1.00f);
            style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.14f, 0.55f, 0.94f, 0.70f);
        }
        break;
    case UiTheme::Light:
        ImGui::StyleColorsLight();
        break;
    case UiTheme::Classic:
        ImGui::StyleColorsClassic();
        break;
    }
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TableRowBgAlt] = style.Colors[ImGuiCol_TableRowBg];
    const float scale = app.useDisplayScale ? app.displayScale : app.uiScale;
    style.ScaleAllSizes(scale);
    ImGui::GetIO().FontGlobalScale = scale;
#ifdef _WIN32
    updateNativeTitleBar(app.theme);
#endif
}

std::string loadUiFont()
{
    std::vector<fs::path> fontPaths;
#ifdef _WIN32
    char* windowsDirectory = nullptr;
    std::size_t windowsDirectorySize = 0;
    if (_dupenv_s(&windowsDirectory, &windowsDirectorySize, "WINDIR") == 0 &&
        windowsDirectory != nullptr && windowsDirectory[0] != '\0')
    {
        const fs::path fontsDirectory = fs::path(windowsDirectory) / "Fonts";
        fontPaths.push_back(fontsDirectory / "segoeui.ttf");
        fontPaths.push_back(fontsDirectory / "arial.ttf");
    }
    std::free(windowsDirectory);
#elif defined(__APPLE__)
    fontPaths.emplace_back("/System/Library/Fonts/Supplemental/Arial.ttf");
    fontPaths.emplace_back("/Library/Fonts/Arial.ttf");
#else
    fontPaths.emplace_back("/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf");
    fontPaths.emplace_back("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    fontPaths.emplace_back("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    fontPaths.emplace_back("/usr/share/fonts/dejavu/DejaVuSans.ttf");
#endif

    for (const fs::path& fontPath : fontPaths)
    {
        std::error_code error;
        if (!fs::is_regular_file(fontPath, error) || error)
            continue;

        const std::string fontPathUtf8 = fontPath.u8string();
        constexpr float fontRasterScale = 2.0f;
        ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPathUtf8.c_str(),
            18.0f * fontRasterScale);
        if (font == nullptr)
            continue;

        font->Scale = 1.0f / fontRasterScale;
        ImGui::GetIO().FontDefault = font;
        return {};
    }

    std::fprintf(stderr, "Could not load an installed system sans-serif UI font\n");
    return "Could not load a system UI font; using the default UI font";
}

fs::path appConfigDirectory()
{
#ifdef _WIN32
    char* configRoot = nullptr;
    std::size_t configRootSize = 0;
    if (_dupenv_s(&configRoot, &configRootSize, "APPDATA") == 0 &&
        configRoot != nullptr && configRoot[0] != '\0')
    {
        const fs::path directory = fs::path(configRoot) / "Astree";
        std::free(configRoot);
        return directory;
    }
    std::free(configRoot);
#elif defined(__APPLE__)
    const char* home = std::getenv("HOME");
    if (home != nullptr && home[0] != '\0')
        return fs::path(home) / "Library" / "Application Support" / "Astree";
#else
    const char* configRoot = std::getenv("XDG_CONFIG_HOME");
    if (configRoot != nullptr && configRoot[0] != '\0')
        return fs::path(configRoot) / "astree";
    const char* home = std::getenv("HOME");
    if (home != nullptr && home[0] != '\0')
        return fs::path(home) / ".config" / "astree";
#endif
    return fs::current_path() / ".astree";
}

void prepareAppConfig(App& app)
{
    fs::path configDirectory = appConfigDirectory();
    std::error_code error;
    fs::create_directories(configDirectory, error);
    if (error)
    {
        std::fprintf(stderr, "Could not create settings directory '%s': %s\n",
            configDirectory.string().c_str(), error.message().c_str());
        app.status = "Could not create the settings directory";
        configDirectory = fs::current_path();
    }

    app.settingsFile = configDirectory / "settings.ini";
    app.imguiIniPath = configDirectory / "AstreeImGui.ini";
    app.imguiIniFilename = app.imguiIniPath.u8string();

    const fs::path legacyIni = fs::current_path() / "AstreeImGui.ini";
    if (legacyIni != app.imguiIniPath)
    {
        const bool hasSavedIni = fs::exists(app.imguiIniPath, error);
        if (error)
        {
            std::fprintf(stderr, "Could not check the saved panel layout: %s\n",
                error.message().c_str());
            app.status = "Could not check the saved panel layout";
            return;
        }
        if (!hasSavedIni)
        {
            const bool hasLegacyIni = fs::exists(legacyIni, error);
            if (error)
            {
                std::fprintf(stderr, "Could not check the legacy panel layout: %s\n",
                    error.message().c_str());
                app.status = "Could not check the legacy panel layout";
                return;
            }
            if (hasLegacyIni)
            {
                fs::copy_file(legacyIni, app.imguiIniPath, fs::copy_options::none, error);
                if (error)
                {
                    std::fprintf(stderr, "Could not migrate panel layout: %s\n",
                        error.message().c_str());
                    app.status = "Could not migrate the saved panel layout";
                }
            }
        }
    }
}

template <typename T>
bool parseSettingValue(const std::string& text, T& value)
{
    std::istringstream stream(text);
    T parsed{};
    if (!(stream >> parsed))
        return false;
    stream >> std::ws;
    if (!stream.eof())
        return false;
    value = parsed;
    return true;
}

std::string loadAppSettings(App& app)
{
    std::ifstream input(app.settingsFile);
    if (!input)
    {
        std::error_code error;
        if (!fs::exists(app.settingsFile, error) && !error)
            return {};
        return "Could not read saved preferences";
    }

    std::unordered_map<std::string, std::string> values;
    std::string line;
    while (std::getline(input, line))
    {
        const std::size_t separator = line.find('=');
        if (separator != std::string::npos)
            values[line.substr(0, separator)] = line.substr(separator + 1);
    }
    if (input.bad())
        return "Could not read saved preferences";

    bool invalidValue = false;
    const auto read = [&](const char* key, auto& value) {
        const auto setting = values.find(key);
        if (setting != values.end() && !parseSettingValue(setting->second, value))
            invalidValue = true;
    };

    int theme = static_cast<int>(app.theme);
    int useDisplayScale = app.useDisplayScale ? 1 : 0;
    float uiScale = app.uiScale;
    int hasWindowPosition = 0;
    int windowMaximized = app.windowMaximized ? 1 : 0;
    int fullscreen = 0;
    int windowX = app.windowX;
    int windowY = app.windowY;
    int windowWidth = app.windowWidth;
    int windowHeight = app.windowHeight;
    double layoutZoom = app.layoutZoom;
    float layoutPanX = app.layoutPan.x;
    float layoutPanY = app.layoutPan.y;
    double spotZoom = app.spotZoom;

    read("theme", theme);
    read("use_display_scale", useDisplayScale);
    read("ui_scale", uiScale);
    read("has_window_position", hasWindowPosition);
    read("window_maximized", windowMaximized);
    read("fullscreen", fullscreen);
    read("window_x", windowX);
    read("window_y", windowY);
    read("window_width", windowWidth);
    read("window_height", windowHeight);
    read("layout_zoom", layoutZoom);
    read("layout_pan_x", layoutPanX);
    read("layout_pan_y", layoutPanY);
    read("spot_zoom", spotZoom);

    if (theme >= static_cast<int>(UiTheme::Dark) &&
        theme <= static_cast<int>(UiTheme::Classic))
        app.theme = static_cast<UiTheme>(theme);
    else
        invalidValue = true;
    if (useDisplayScale == 0 || useDisplayScale == 1)
        app.useDisplayScale = useDisplayScale != 0;
    else
        invalidValue = true;
    if (std::isfinite(uiScale) && uiScale >= 0.75f && uiScale <= 2.0f)
        app.uiScale = uiScale;
    else
        invalidValue = true;
    if (hasWindowPosition == 0 || hasWindowPosition == 1)
        app.hasWindowPosition = hasWindowPosition != 0;
    else
        invalidValue = true;
    if ((windowMaximized == 0 || windowMaximized == 1) &&
        (fullscreen == 0 || fullscreen == 1))
        app.windowMaximized = windowMaximized != 0 || fullscreen != 0;
    else
        invalidValue = true;
    if (windowWidth > 0 && windowWidth <= 32768 &&
        windowHeight > 0 && windowHeight <= 32768)
    {
        app.windowWidth = windowWidth;
        app.windowHeight = windowHeight;
    }
    else
        invalidValue = true;
    if (windowX >= -10000 && windowX <= 100000 &&
        windowY >= -10000 && windowY <= 100000)
    {
        app.windowX = windowX;
        app.windowY = windowY;
    }
    else
    {
        app.hasWindowPosition = false;
        invalidValue = true;
    }
    if (std::isfinite(layoutZoom) && layoutZoom >= 0.1 &&
        layoutZoom <= kMaxOpticalLayoutZoom)
        app.layoutZoom = layoutZoom;
    else
        invalidValue = true;
    if (std::isfinite(layoutPanX) && std::abs(layoutPanX) <= 1000000.0f &&
        std::isfinite(layoutPanY) && std::abs(layoutPanY) <= 1000000.0f)
        app.layoutPan = ImVec2(layoutPanX, layoutPanY);
    else
        invalidValue = true;
    if (std::isfinite(spotZoom) && spotZoom >= 0.1 && spotZoom <= 50.0)
        app.spotZoom = spotZoom;
    else
        invalidValue = true;

    app.recentFiles.clear();
    for (std::size_t i = 0; i < kMaxRecentFiles; ++i)
    {
        const auto pathSetting = values.find("recent_path_" + std::to_string(i));
        if (pathSetting == values.end() || pathSetting->second.empty())
            continue;
        int zmx = 0;
        const auto typeSetting = values.find("recent_zmx_" + std::to_string(i));
        if (typeSetting != values.end() && !parseSettingValue(typeSetting->second, zmx))
        {
            invalidValue = true;
            continue;
        }
        if (zmx != 0 && zmx != 1)
        {
            invalidValue = true;
            continue;
        }
        app.recentFiles.push_back({ pathSetting->second, zmx != 0 });
    }

    if (invalidValue)
        return "Some saved preferences were invalid and were ignored";
    return {};
}

bool saveAppSettings(const App& app)
{
    std::ofstream output(app.settingsFile, std::ios::trunc);
    if (!output)
    {
        std::fprintf(stderr, "Could not save preferences to '%s'\n",
            app.settingsFile.string().c_str());
        return false;
    }
    output << "theme=" << static_cast<int>(app.theme) << '\n'
        << "use_display_scale=" << (app.useDisplayScale ? 1 : 0) << '\n'
        << "ui_scale=" << std::setprecision(9) << app.uiScale << '\n'
        << "has_window_position=" << (app.hasWindowPosition ? 1 : 0) << '\n'
        << "window_maximized=" << (app.windowMaximized ? 1 : 0) << '\n'
        << "window_x=" << app.windowX << '\n'
        << "window_y=" << app.windowY << '\n'
        << "window_width=" << app.windowWidth << '\n'
        << "window_height=" << app.windowHeight << '\n'
        << "layout_zoom=" << std::setprecision(17) << app.layoutZoom << '\n'
        << "layout_pan_x=" << std::setprecision(9) << app.layoutPan.x << '\n'
        << "layout_pan_y=" << app.layoutPan.y << '\n'
        << "spot_zoom=" << std::setprecision(17) << app.spotZoom << '\n';
    const std::size_t recentCount = std::min(app.recentFiles.size(), kMaxRecentFiles);
    for (std::size_t i = 0; i < recentCount; ++i)
    {
        output << "recent_path_" << i << '=' << app.recentFiles[i].path << '\n'
            << "recent_zmx_" << i << '=' << (app.recentFiles[i].zmx ? 1 : 0) << '\n';
    }
    output.flush();
    if (!output)
    {
        std::fprintf(stderr, "Could not write preferences to '%s'\n",
            app.settingsFile.string().c_str());
        return false;
    }
    return true;
}

void recordWindowedGeometry(GLFWwindow* window, App& app)
{
    if (app.windowMaximized)
        return;
    int windowX = 0;
    int windowY = 0;
    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowPos(window, &windowX, &windowY);
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    if (windowX >= -10000 && windowX <= 100000 &&
        windowY >= -10000 && windowY <= 100000 &&
        windowWidth > 0 && windowWidth <= 32768 &&
        windowHeight > 0 && windowHeight <= 32768)
    {
        app.windowX = windowX;
        app.windowY = windowY;
        app.windowWidth = windowWidth;
        app.windowHeight = windowHeight;
        app.hasWindowPosition = true;
    }
    else
    {
        app.hasWindowPosition = false;
    }
}

void toggleMaximizedWindow(GLFWwindow* window, App& app)
{
    if (app.windowMaximized)
    {
        app.windowMaximized = false;
        glfwRestoreWindow(window);
        return;
    }

    recordWindowedGeometry(window, app);
    app.windowMaximized = true;
    glfwMaximizeWindow(window);
}

void resetUiSettings(App& app)
{
    app.theme = UiTheme::Dark;
    app.useDisplayScale = true;
    app.uiScale = 1.0f;
    app.initializeDockLayout = true;
    app.hasWindowPosition = false;
    app.windowMaximized = false;
    app.windowX = 100;
    app.windowY = 100;
    app.windowWidth = 1440;
    app.windowHeight = 900;
    app.layoutZoom = 1.0;
    app.layoutPan = ImVec2(0.0f, 0.0f);
    app.spotZoom = 1.0;

    app.showFilePicker = false;
    app.showScale = false;
    app.showRevert = false;
    app.showMediumManager = false;
    app.showCloneEditor = false;
    app.showAbout = false;

    GLFWwindow* window = glfwGetCurrentContext();
    glfwRestoreWindow(window);
    glfwSetWindowSize(window, app.windowWidth, app.windowHeight);
    glfwSetWindowPos(window, app.windowX, app.windowY);
    app.windowMaximized = true;
    glfwMaximizeWindow(window);

    ImGui::ClearIniSettings();
    std::error_code error;
    fs::remove(app.imguiIniPath, error);
    if (error)
    {
        std::fprintf(stderr, "Could not remove saved panel layout: %s\n",
            error.message().c_str());
        app.status = "Could not fully reset saved UI settings";
    }
    else
    {
        app.status = "UI settings reset";
    }
    applyTheme(app);
    saveAppSettings(app);
}

void onWindowPosition(GLFWwindow* window, int, int)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app != nullptr)
        recordWindowedGeometry(window, *app);
}

void onWindowSize(GLFWwindow* window, int, int)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app != nullptr)
        recordWindowedGeometry(window, *app);
}

void onWindowMaximize(GLFWwindow* window, int maximized)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app == nullptr)
        return;
    app->windowMaximized = maximized == GLFW_TRUE;
    if (!app->windowMaximized)
        recordWindowedGeometry(window, *app);
}
}
