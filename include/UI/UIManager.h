#pragma once

#include <memory>
#include <imgui.h>

class Application;
class Toolbar;
class Sidebar;
class PropertiesPanel;
class FileBrowser;

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

private:
    Application* m_Application;

    std::unique_ptr<Toolbar> m_Toolbar;
    std::unique_ptr<Sidebar> m_Sidebar;
    std::unique_ptr<PropertiesPanel> m_PropertiesPanel;
    std::unique_ptr<FileBrowser> m_FileBrowser;

    bool m_ShowDemoWindow;
};

