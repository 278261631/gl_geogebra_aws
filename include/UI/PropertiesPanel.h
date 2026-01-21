#pragma once

#include <imgui.h>

class UIManager;

class PropertiesPanel {
public:
    PropertiesPanel(UIManager* uiManager);
    ~PropertiesPanel();

    void Render();

private:
    UIManager* m_UIManager;
};

