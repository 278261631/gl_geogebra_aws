#pragma once

#include <imgui.h>

class UIManager;

class Toolbar {
public:
    Toolbar(UIManager* uiManager);
    ~Toolbar();

    void Render();

private:
    UIManager* m_UIManager;

    enum class Tool {
        Select,
        Point,
        Line,
        Plane,
        Sphere,
        Cube,
        Move,
        Rotate,
        Scale
    };

    Tool m_CurrentTool;
};

