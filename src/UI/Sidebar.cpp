#include "UI/Sidebar.h"
#include "UI/UIManager.h"
#include "Application.h"
#include "GeometryObject.h"
#include <imgui.h>

Sidebar::Sidebar(UIManager* uiManager)
    : m_UIManager(uiManager)
    , m_SelectedObjectIndex(-1)
{
}

Sidebar::~Sidebar() {
}

void Sidebar::Render() {
    ImGui::Begin("Objects", nullptr, ImGuiWindowFlags_NoResize);
    ImGui::SetWindowSize(ImVec2(250, 600));
    ImGui::SetWindowPos(ImVec2(10, 420));

    ImGui::Text("Scene Objects");
    ImGui::Separator();

    auto& objects = m_UIManager->GetApplication()->GetGeometryObjects();
    
    for (size_t i = 0; i < objects.size(); ++i) {
        auto& obj = objects[i];
        
        ImGui::PushID((int)i);
        
        bool isVisible = obj->IsVisible();
        if (ImGui::Checkbox("##visible", &isVisible)) {
            obj->SetVisible(isVisible);
        }
        
        ImGui::SameLine();
        
        if (ImGui::Selectable(obj->GetName().c_str(), m_SelectedObjectIndex == (int)i)) {
            m_SelectedObjectIndex = (int)i;
        }
        
        ImGui::PopID();
    }

    ImGui::Separator();
    
    if (ImGui::Button("Delete Selected", ImVec2(230, 30))) {
        if (m_SelectedObjectIndex >= 0 && m_SelectedObjectIndex < (int)objects.size()) {
            m_UIManager->GetApplication()->RemoveGeometryObject(objects[m_SelectedObjectIndex]);
            m_SelectedObjectIndex = -1;
        }
    }

    ImGui::End();
}

