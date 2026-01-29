#include "UI/Toolbar.h"
#include "UI/UIManager.h"
#include "UI/FileBrowser.h"
#include "UI/LabelDataBrowser.h"
#include "Application.h"
#include "Geometry/Point.h"
#include "Geometry/Line.h"
#include "Geometry/Sphere.h"
#include "Geometry/Cube.h"
#include "Geometry/Plane.h"
#include <imgui.h>

Toolbar::Toolbar(UIManager* uiManager)
    : m_UIManager(uiManager)
    , m_CurrentTool(Tool::Select)
{
}

Toolbar::~Toolbar() {
}

void Toolbar::Render() {
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowSize(ImVec2(200, 400));
    ImGui::SetWindowPos(ImVec2(10, 10));

    ImGui::Text("Tools");
    ImGui::Separator();

    if (ImGui::Button("Select", ImVec2(180, 30))) {
        m_CurrentTool = Tool::Select;
    }

    ImGui::Separator();
    ImGui::Text("Create Objects");

    if (ImGui::Button("Point", ImVec2(180, 30))) {
        auto point = std::make_shared<Point>(glm::vec3(0.0f, 0.0f, 0.0f));
        point->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        point->SetName("Point");
        point->Initialize();
        m_UIManager->GetApplication()->AddGeometryObject(point);
    }

    if (ImGui::Button("Line", ImVec2(180, 30))) {
        auto line = std::make_shared<Line>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f));
        line->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        line->SetName("Line");
        line->Initialize();
        m_UIManager->GetApplication()->AddGeometryObject(line);
    }

    if (ImGui::Button("Plane", ImVec2(180, 30))) {
        auto plane = std::make_shared<Plane>(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
        plane->SetColor(glm::vec4(0.5f, 0.5f, 0.8f, 0.5f));
        plane->SetName("Plane");
        plane->Initialize();
        m_UIManager->GetApplication()->AddGeometryObject(plane);
    }

    if (ImGui::Button("Sphere", ImVec2(180, 30))) {
        auto sphere = std::make_shared<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
        sphere->SetColor(glm::vec4(0.2f, 0.5f, 0.9f, 1.0f));
        sphere->SetName("Sphere");
        sphere->Initialize();
        m_UIManager->GetApplication()->AddGeometryObject(sphere);
    }

    if (ImGui::Button("Cube", ImVec2(180, 30))) {
        auto cube = std::make_shared<Cube>(glm::vec3(0.0f, 0.5f, 0.0f), 1.0f);
        cube->SetColor(glm::vec4(0.9f, 0.3f, 0.2f, 1.0f));
        cube->SetName("Cube");
        cube->Initialize();
        m_UIManager->GetApplication()->AddGeometryObject(cube);
    }

    ImGui::Separator();
    ImGui::Text("Transform");

    if (ImGui::Button("Move", ImVec2(180, 30))) {
        m_CurrentTool = Tool::Move;
    }

    if (ImGui::Button("Rotate", ImVec2(180, 30))) {
        m_CurrentTool = Tool::Rotate;
    }

    if (ImGui::Button("Scale", ImVec2(180, 30))) {
        m_CurrentTool = Tool::Scale;
    }

    ImGui::Separator();
    ImGui::Text("File");

    if (ImGui::Button("Browse Files", ImVec2(180, 30))) {
        m_UIManager->GetFileBrowser()->Open();
    }

    if (ImGui::Button("Browse Label Data", ImVec2(180, 30))) {
        m_UIManager->GetLabelDataBrowser()->Open();
    }

    ImGui::End();
}

