#include "UI/UIManager.h"
#include "UI/Toolbar.h"
#include "UI/Sidebar.h"
#include "UI/PropertiesPanel.h"
#include "Application.h"
#include "Window.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

UIManager::UIManager(Application* app)
    : m_Application(app)
    , m_ShowDemoWindow(false)
{
}

UIManager::~UIManager() {
}

bool UIManager::Initialize() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Docking is not available in ImGui 1.89.9
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    GLFWwindow* window = m_Application->GetWindow()->GetNativeWindow();
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "Failed to initialize ImGui GLFW" << std::endl;
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        std::cerr << "Failed to initialize ImGui OpenGL3" << std::endl;
        return false;
    }

    m_Toolbar = std::make_unique<Toolbar>(this);
    m_Sidebar = std::make_unique<Sidebar>(this);
    m_PropertiesPanel = std::make_unique<PropertiesPanel>(this);

    std::cout << "UI Manager initialized" << std::endl;
    return true;
}

void UIManager::Shutdown() {
    m_PropertiesPanel.reset();
    m_Sidebar.reset();
    m_Toolbar.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::Render() {
    m_Toolbar->Render();
    m_Sidebar->Render();
    m_PropertiesPanel->Render();

    if (m_ShowDemoWindow) {
        ImGui::ShowDemoWindow(&m_ShowDemoWindow);
    }
}

