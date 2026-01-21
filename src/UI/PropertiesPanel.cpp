#include "UI/PropertiesPanel.h"
#include "UI/UIManager.h"
#include "Application.h"
#include "GeometryObject.h"
#include <imgui.h>

PropertiesPanel::PropertiesPanel(UIManager* uiManager)
    : m_UIManager(uiManager)
{
}

PropertiesPanel::~PropertiesPanel() {
}

void PropertiesPanel::Render() {
    ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoResize);
    
    int windowWidth = m_UIManager->GetApplication()->GetWindow()->GetWidth();
    ImGui::SetWindowSize(ImVec2(300, 600));
    ImGui::SetWindowPos(ImVec2((float)(windowWidth - 310), 10));

    ImGui::Text("Object Properties");
    ImGui::Separator();

    auto& objects = m_UIManager->GetApplication()->GetGeometryObjects();
    
    if (!objects.empty()) {
        auto& obj = objects[0];
        
        ImGui::Text("Name: %s", obj->GetName().c_str());
        
        glm::vec3 pos = obj->GetPosition();
        if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
            obj->SetPosition(pos);
        }
        
        glm::vec3 rot = obj->GetRotation();
        if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) {
            obj->SetRotation(rot);
        }
        
        glm::vec3 scale = obj->GetScale();
        if (ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.1f, 10.0f)) {
            obj->SetScale(scale);
        }
        
        glm::vec4 color = obj->GetColor();
        if (ImGui::ColorEdit4("Color", &color.x)) {
            obj->SetColor(color);
        }
    }

    ImGui::Separator();
    ImGui::Text("Camera");
    
    auto camera = m_UIManager->GetApplication()->GetCamera();
    glm::vec3 camPos = camera->GetPosition();
    ImGui::Text("Position: %.2f, %.2f, %.2f", camPos.x, camPos.y, camPos.z);
    
    glm::vec3 camTarget = camera->GetTarget();
    ImGui::Text("Target: %.2f, %.2f, %.2f", camTarget.x, camTarget.y, camTarget.z);

    ImGui::End();
}

