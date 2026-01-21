#pragma once

#include <memory>
#include <imgui.h>

class Application;
class Toolbar;
class Sidebar;
class PropertiesPanel;

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

private:
    Application* m_Application;

    std::unique_ptr<Toolbar> m_Toolbar;
    std::unique_ptr<Sidebar> m_Sidebar;
    std::unique_ptr<PropertiesPanel> m_PropertiesPanel;

    bool m_ShowDemoWindow;
};

