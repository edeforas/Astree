#pragma once

#include "DeviceOptimizer.h"
#include "OpticalDevice.h"
#include "imgui.h"

#include <array>
#include <cstddef>
#include <filesystem>
#include <future>
#include <string>
#include <vector>

struct GLFWwindow;

namespace astree_imgui_app
{
namespace fs = std::filesystem;

inline constexpr int kMaxOptimizerParameters = 10;
inline constexpr int kInitialOptimizerParameterRows = 4;
inline constexpr std::size_t kMaxRecentFiles = 10;
inline constexpr double kMaxOpticalLayoutZoom = 5000.0;
inline constexpr const char* kParameterNames[] = {
    "", "RCurv", "Conic", "Thick", "Z", "R4", "R6", "R8", "R10"
};
inline constexpr const char* kColorNames[] = {
    "IR", "Red", "YellowBlack", "Yellow", "Green", "Blue", "UV"
};
inline constexpr const char* kCloneParameters[] = {
    "Radius curvature", "Conic", "Position / thickness", "Diameter",
    "InnerDiameter", "R4", "R6", "R8", "R10"
};

struct OptimizerRow
{
    bool enabled = true;
    int surface = 1;
    int parameter = 0;
    double minimum = 0.0;
    double maximum = 0.0;
    bool automaticBounds = true;
    char minimumText[32] = "auto";
    char maximumText[32] = "auto";
};

struct OptimizerCompletion
{
    OptimizerResult result;
    OpticalDevice device;
};

enum class PendingAction
{
    None,
    NewDesign,
    LoadDesign,
    ImportZmx,
    Quit
};

enum class UiTheme
{
    Dark,
    Light,
    Classic
};

struct RecentFile
{
    std::string path;
    bool zmx = false;
};

struct App
{
    OpticalDevice device;
    std::string fileName;
    std::string status = "Ready";
    bool dirty = false;
    bool showFilePicker = false;
    bool saveAs = false;
    bool importZmx = false;
    bool showScale = false;
    bool showRevert = false;
    bool showMediumManager = false;
    bool showCloneEditor = false;
    bool showAbout = false;
    bool confirmResetSettings = false;
    UiTheme theme = UiTheme::Dark;
    float displayScale = 1.0f;
    float uiScale = 1.0f;
    bool useDisplayScale = true;
    bool initializeDockLayout = true;
    bool hasWindowPosition = false;
    bool windowMaximized = true;
    int windowX = 0;
    int windowY = 0;
    int windowWidth = 1440;
    int windowHeight = 900;
    bool confirmPendingAction = false;
    PendingAction pendingAction = PendingAction::None;
    std::string pendingPath;
    fs::path settingsFile;
    fs::path imguiIniPath;
    std::string imguiIniFilename;
    std::vector<RecentFile> recentFiles;
    fs::path browserDirectory = fs::current_path();
    char fileNameBuffer[512] = {};
    char noteBuffer[4096] = {};
    std::array<OptimizerRow, kMaxOptimizerParameters> optimizerRows;
    int optimizerRowCount = kInitialOptimizerParameterRows;
    std::future<OptimizerCompletion> optimizerTask;
    bool optimizerRunning = false;
    int optimizerMethod = 0;
    int meritFunction = 0;
    double minimumVignetting = 100.0;
    int selectedImageQualityRow = 0;
    int selectedSurface = -1;
    double offAxisPercent = 0.0;
    double fov = 0.0;
    int intermediateAngles = 3;
    std::array<bool, 7> colors = {};
    bool relativeConvention = true;
    bool autofocus = false;
    bool autoCurvature = false;
    bool displayAspheric = true;
    bool displayInnerDiameter = true;
    bool displayComments = true;
    bool displayTilt = false;
    int cloneSurface = 2;
    int cloneReference = 1;
    int cloneParameter = 0;
    bool cloneNegative = false;
    int revertFirst = 1;
    int revertLast = 1;
    int scaleMode = 0;
    double scaleFactor = 1.0;
    double scalePercentage = 100.0;
    double scaleOldSize = 1.0;
    double scaleNewSize = 1.0;
    bool scaleDiameter = true;
    bool scaleFocal = true;
    double layoutZoom = 1.0;
    ImVec2 layoutPan = ImVec2(0.0f, 0.0f);
    double spotZoom = 1.0;
};

void applyTheme(const App& app);
std::string loadUiFont();
void prepareAppConfig(App& app);
std::string loadAppSettings(App& app);
bool saveAppSettings(const App& app);
void recordWindowedGeometry(GLFWwindow* window, App& app);
void toggleMaximizedWindow(GLFWwindow* window, App& app);
void resetUiSettings(App& app);
void onWindowPosition(GLFWwindow* window, int x, int y);
void onWindowSize(GLFWwindow* window, int width, int height);
void onWindowMaximize(GLFWwindow* window, int maximized);
void onWindowClose(GLFWwindow* window);
void onWindowContentScale(GLFWwindow* window, float xScale, float yScale);

std::string lowerCase(std::string value);
void markChanged(App& app, const char* message = "Design modified");
void loadOptimizerSettings(App& app);
void saveOptimizerSettings(App& app);
void fitOpticalLayout(App& app);
void syncUiFromDevice(App& app);
void loadGlassCatalogs(App& app);
bool loadDesign(App& app, const fs::path& path, bool zmx);
bool saveDesign(App& app, const fs::path& path);
void executePendingAction(App& app);
void requestAction(App& app, PendingAction action, const fs::path& path = {});
bool acceptFileSelection(App& app, fs::path target);
void openFilePicker(App& app, bool save, bool zmx);
void drawFilePicker(App& app);
void drawFileMenu(App& app);
void drawOpticalPanels(App& app);
void drawUi(App& app);
int runApp(int argc, char** argv);
}
