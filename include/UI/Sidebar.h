#pragma once

#include <imgui.h>

class UIManager;

class Sidebar {
public:
    Sidebar(UIManager* uiManager);
    ~Sidebar();

    void Render();

private:
    UIManager* m_UIManager;
    int m_SelectedObjectIndex;
};

