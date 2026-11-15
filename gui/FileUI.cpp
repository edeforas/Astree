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
#include <shobjidl.h>
#endif

#include "App.h"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <vector>

namespace astree_imgui_app
{
#ifdef _WIN32
enum class NativeDialogResult
{
    Unavailable,
    Cancelled,
    Selected,
    Failed
};

struct ComReleaser
{
    template <typename T>
    void operator()(T* object) const
    {
        if (object != nullptr)
            object->Release();
    }
};

struct ComApartment
{
    HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool initialized = SUCCEEDED(result);

    ~ComApartment()
    {
        if (initialized)
            CoUninitialize();
    }
};

struct CoTaskMemReleaser
{
    void operator()(wchar_t* memory) const
    {
        CoTaskMemFree(memory);
    }
};

NativeDialogResult showNativeFileDialog(bool save, bool zmx, const fs::path& initialPath,
    fs::path& selectedPath, HRESULT& result)
{
    ComApartment apartment;
    if (FAILED(apartment.result) && apartment.result != RPC_E_CHANGED_MODE)
    {
        result = apartment.result;
        return NativeDialogResult::Failed;
    }

    IFileDialog* rawDialog = nullptr;
    result = CoCreateInstance(save ? CLSID_FileSaveDialog : CLSID_FileOpenDialog, nullptr,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&rawDialog));
    std::unique_ptr<IFileDialog, ComReleaser> dialog(rawDialog);
    if (FAILED(result))
        return NativeDialogResult::Failed;

    const COMDLG_FILTERSPEC filter = zmx
        ? COMDLG_FILTERSPEC{ L"Zemax lens files (*.zmx)", L"*.zmx" }
        : COMDLG_FILTERSPEC{ L"Astree designs (*.astree)", L"*.astree" };
    result = dialog->SetFileTypes(1, &filter);
    if (SUCCEEDED(result))
        result = dialog->SetFileTypeIndex(1);
    if (SUCCEEDED(result) && save)
        result = dialog->SetDefaultExtension(zmx ? L"zmx" : L"astree");
    if (SUCCEEDED(result) && save && !initialPath.empty())
        result = dialog->SetFileName(initialPath.filename().c_str());

    std::unique_ptr<IShellItem, ComReleaser> folder;
    if (SUCCEEDED(result))
    {
        const fs::path initialFolder = save && !initialPath.empty() && initialPath.has_parent_path()
            ? initialPath.parent_path() : fs::current_path();
        IShellItem* rawFolder = nullptr;
        result = SHCreateItemFromParsingName(initialFolder.c_str(), nullptr,
            IID_PPV_ARGS(&rawFolder));
        folder.reset(rawFolder);
        if (SUCCEEDED(result))
            result = dialog->SetFolder(folder.get());
    }
    if (SUCCEEDED(result))
    {
        DWORD options = 0;
        result = dialog->GetOptions(&options);
        if (SUCCEEDED(result))
            result = dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST |
                (save ? FOS_OVERWRITEPROMPT : FOS_FILEMUSTEXIST));
    }
    if (SUCCEEDED(result))
    {
        result = dialog->SetTitle(zmx ? L"Import ZMX" : (save ? L"Save design as" : L"Open design"));
        if (SUCCEEDED(result))
            result = dialog->Show(glfwGetWin32Window(glfwGetCurrentContext()));
    }

    NativeDialogResult dialogResult = NativeDialogResult::Failed;
    if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED))
        dialogResult = NativeDialogResult::Cancelled;
    else if (SUCCEEDED(result))
    {
        IShellItem* rawItem = nullptr;
        result = dialog->GetResult(&rawItem);
        std::unique_ptr<IShellItem, ComReleaser> item(rawItem);
        if (SUCCEEDED(result))
        {
            PWSTR rawPath = nullptr;
            result = item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath);
            if (SUCCEEDED(result))
            {
                std::unique_ptr<wchar_t, CoTaskMemReleaser> path(rawPath);
                selectedPath = path.get();
                dialogResult = NativeDialogResult::Selected;
            }
        }
    }

    return dialogResult;
}
#endif

void openFilePicker(App& app, bool save, bool zmx)
{
    app.saveAs = save;
    app.importZmx = zmx;
    app.browserDirectory = fs::current_path();
    const fs::path initialPath = save && !app.fileName.empty() ? fs::path(app.fileName) : fs::path();
#ifdef _WIN32
    fs::path selectedPath;
    HRESULT result = S_OK;
    const NativeDialogResult dialogResult = showNativeFileDialog(save, zmx, initialPath,
        selectedPath, result);
    if (dialogResult == NativeDialogResult::Selected)
    {
        acceptFileSelection(app, selectedPath);
        return;
    }
    if (dialogResult == NativeDialogResult::Cancelled)
        return;
    if (dialogResult == NativeDialogResult::Failed)
        app.status = "Native file dialog failed (HRESULT " +
            std::to_string(static_cast<unsigned long>(result)) + "); using built-in picker";
#endif
    const std::string initialName = initialPath.empty() ? "" : initialPath.filename().string();
    std::snprintf(app.fileNameBuffer, sizeof(app.fileNameBuffer), "%s", initialName.c_str());
    app.showFilePicker = true;
}

void drawFilePicker(App& app)
{
    const char* title = app.importZmx ? "Import ZMX" : (app.saveAs ? "Save design as" : "Open design");
    if (app.showFilePicker)
    {
        ImGui::OpenPopup(title);
        app.showFilePicker = false;
    }
    if (!ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    ImGui::TextWrapped("%s", app.browserDirectory.string().c_str());
    ImGui::SetNextItemWidth(420.0f);
    ImGui::InputText("File name", app.fileNameBuffer, sizeof(app.fileNameBuffer));
    if (ImGui::BeginChild("file-list", ImVec2(520.0f, 260.0f), true))
    {
        std::error_code error;
        std::vector<fs::directory_entry> entries;
        for (const auto& entry : fs::directory_iterator(app.browserDirectory, error))
        {
            if (error)
                break;
            if (entry.is_directory() ||
                (entry.is_regular_file() &&
                 (app.importZmx ? lowerCase(entry.path().extension().string()) == ".zmx"
                                : lowerCase(entry.path().extension().string()) == ".astree")))
                entries.push_back(entry);
        }
        std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
            if (a.is_directory() != b.is_directory()) return a.is_directory();
            return a.path().filename().string() < b.path().filename().string();
        });
        if (app.browserDirectory.has_parent_path() && ImGui::Selectable("[..]"))
            app.browserDirectory = app.browserDirectory.parent_path();
        for (const auto& entry : entries)
        {
            const std::string name = entry.path().filename().string() +
                (entry.is_directory() ? "\\" : "");
            if (!ImGui::Selectable(name.c_str()))
                continue;
            if (entry.is_directory())
                app.browserDirectory = entry.path();
            else
                std::snprintf(app.fileNameBuffer, sizeof(app.fileNameBuffer), "%s",
                    entry.path().filename().string().c_str());
        }
    }
    ImGui::EndChild();

    if (ImGui::Button(app.importZmx ? "Import" : (app.saveAs ? "Save" : "Open")))
    {
        fs::path target(app.fileNameBuffer);
        if (target.is_relative())
            target = app.browserDirectory / target;
        if (acceptFileSelection(app, target))
            ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
}

void drawFileMenu(App& app)
{
    if (!ImGui::BeginMainMenuBar())
        return;
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New"))
            requestAction(app, PendingAction::NewDesign);
        if (ImGui::MenuItem("Open..."))
            openFilePicker(app, false, false);
        if (ImGui::MenuItem("Import ZMX..."))
            openFilePicker(app, false, true);
        if (ImGui::BeginMenu("Recent files"))
        {
            if (app.recentFiles.empty())
                ImGui::TextDisabled("No recent files");
            else
            {
                for (const RecentFile& recent : app.recentFiles)
                {
                    const fs::path path = fs::u8path(recent.path);
                    std::string label = path.filename().u8string();
                    const std::string parent = path.parent_path().u8string();
                    if (!parent.empty())
                        label += " (" + parent + ")";
                    if (ImGui::MenuItem(label.c_str()))
                        requestAction(app, recent.zmx ? PendingAction::ImportZmx
                            : PendingAction::LoadDesign, path);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Clear recent files"))
                {
                    app.recentFiles.clear();
                    if (!saveAppSettings(app))
                        app.status = "Could not save recent files";
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Reload", nullptr, false, !app.fileName.empty()))
            requestAction(app, PendingAction::LoadDesign, app.fileName);
        if (ImGui::MenuItem("Save", "Ctrl+S", false, !app.fileName.empty()))
            saveDesign(app, app.fileName);
        if (ImGui::MenuItem("Save As..."))
            openFilePicker(app, true, false);
        if (ImGui::MenuItem("Close design"))
            requestAction(app, PendingAction::NewDesign);
        ImGui::Separator();
        if (ImGui::MenuItem("Quit"))
            requestAction(app, PendingAction::Quit);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools"))
    {
        if (ImGui::MenuItem("Medium manager")) app.showMediumManager = true;
        if (ImGui::MenuItem("Clone parameter...")) app.showCloneEditor = true;
        if (ImGui::MenuItem("Scale design")) app.showScale = true;
        if (ImGui::MenuItem("Revert design")) app.showRevert = true;
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
        if (ImGui::MenuItem("Reset Settings..."))
            app.confirmResetSettings = true;
        if (ImGui::MenuItem("Fit all"))
        {
            fitOpticalLayout(app);
            app.spotZoom = 1.0;
        }
        if (ImGui::MenuItem("Zoom in"))
            app.layoutZoom = std::min(kMaxOpticalLayoutZoom, app.layoutZoom * 1.25);
        if (ImGui::MenuItem("Zoom out"))
            app.layoutZoom = std::max(0.1, app.layoutZoom * 0.8);
        ImGui::Separator();
        if (ImGui::BeginMenu("Theme"))
        {
            if (ImGui::MenuItem("White / Light", nullptr, app.theme == UiTheme::Light))
            {
                app.theme = UiTheme::Light;
                applyTheme(app);
            }
            if (ImGui::MenuItem("Dark", nullptr, app.theme == UiTheme::Dark))
            {
                app.theme = UiTheme::Dark;
                applyTheme(app);
            }
            if (ImGui::MenuItem("Classic", nullptr, app.theme == UiTheme::Classic))
            {
                app.theme = UiTheme::Classic;
                applyTheme(app);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("UI scale"))
        {
            const float effectiveScale = app.useDisplayScale ? app.displayScale : app.uiScale;
            ImGui::TextDisabled("Effective scale: %.0f%%", effectiveScale * 100.0f);
            if (ImGui::MenuItem("Automatic (display)", nullptr, app.useDisplayScale))
            {
                app.useDisplayScale = true;
                applyTheme(app);
            }
            ImGui::Separator();
            const float scales[] = { 0.75f, 1.0f, 1.25f, 1.5f, 1.75f, 2.0f };
            const char* labels[] = { "75%", "100%", "125%", "150%", "175%", "200%" };
            for (std::size_t i = 0; i < std::size(scales); ++i)
            {
                if (ImGui::MenuItem(labels[i], nullptr,
                    !app.useDisplayScale && std::abs(app.uiScale - scales[i]) < 0.001f))
                {
                    app.useDisplayScale = false;
                    app.uiScale = scales[i];
                    applyTheme(app);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("About Astree")) app.showAbout = true;
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}
}
