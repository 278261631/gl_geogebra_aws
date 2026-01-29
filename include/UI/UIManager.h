#pragma once

#include <memory>
#include <imgui.h>

class Application;
class Toolbar;
class Sidebar;
class PropertiesPanel;
class FileBrowser;
class LabelDataBrowser;

class UIManager {
public:
    UIManager(Application* app);
    ~UIManager();

    bool Initialize();
    void Shutdown();

    void BeginFrame();
    void EndFrame();
    void Render();

    Application* GetApplication() { return m_Application; }
    FileBrowser* GetFileBrowser() { return m_FileBrowser.get(); }
    LabelDataBrowser* GetLabelDataBrowser() { return m_LabelDataBrowser.get(); }

private:
    Application* m_Application;

    std::unique_ptr<Toolbar> m_Toolbar;
    std::unique_ptr<Sidebar> m_Sidebar;
    std::unique_ptr<PropertiesPanel> m_PropertiesPanel;
    std::unique_ptr<FileBrowser> m_FileBrowser;
    std::unique_ptr<LabelDataBrowser> m_LabelDataBrowser;

    bool m_ShowDemoWindow;
};

