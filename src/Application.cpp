#include "Application.h"
#include "GeometryObject.h"
#include "Geometry/Sphere.h"
#include "Geometry/Cube.h"
#include <iostream>
#include <GLFW/glfw3.h>

Application::Application()
    : m_Running(false)
    , m_LastFrameTime(0.0f)
{
}

Application::~Application() {
}

bool Application::Initialize() {
    // Create window
    m_Window = std::make_unique<Window>(1600, 900, "GeoGebra 3D - OpenGL");
    if (!m_Window->Initialize()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return false;
    }

    // Create renderer
    m_Renderer = std::make_unique<Renderer>();
    if (!m_Renderer->Initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    // Create camera
    m_Camera = std::make_unique<Camera>(45.0f, m_Window->GetAspectRatio());
    m_Camera->SetPosition(glm::vec3(10.0f, 10.0f, 10.0f));
    m_Camera->SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));

    // Create input handler
    m_InputHandler = std::make_unique<InputHandler>(m_Window.get(), m_Camera.get());
    m_InputHandler->Initialize();

    // Create UI manager
    m_UIManager = std::make_unique<UIManager>(this);
    if (!m_UIManager->Initialize()) {
        std::cerr << "Failed to initialize UI" << std::endl;
        return false;
    }

    // Create grid and axes
    m_Grid = std::make_unique<Grid>(20.0f, 20);
    m_Grid->Initialize();

    m_Axes = std::make_unique<Axes>(5.0f);
    m_Axes->Initialize();

    // Add some default objects
    auto sphere = std::make_shared<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
    sphere->SetColor(glm::vec4(0.2f, 0.5f, 0.9f, 1.0f));
    sphere->SetName("Sphere 1");
    sphere->Initialize();
    AddGeometryObject(sphere);

    auto cube = std::make_shared<Cube>(glm::vec3(3.0f, 0.5f, 0.0f), 1.0f);
    cube->SetColor(glm::vec4(0.9f, 0.3f, 0.2f, 1.0f));
    cube->SetName("Cube 1");
    cube->Initialize();
    AddGeometryObject(cube);

    m_Running = true;
    m_LastFrameTime = (float)glfwGetTime();

    std::cout << "Application initialized successfully" << std::endl;
    return true;
}

void Application::Run() {
    while (m_Running && !m_Window->ShouldClose()) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - m_LastFrameTime;
        m_LastFrameTime = currentTime;

        Update(deltaTime);
        Render();

        m_Window->SwapBuffers();
        m_Window->PollEvents();
    }
}

void Application::Shutdown() {
    m_GeometryObjects.clear();
    m_Axes.reset();
    m_Grid.reset();
    m_UIManager.reset();
    m_InputHandler.reset();
    m_Camera.reset();
    m_Renderer.reset();
    m_Window.reset();

    std::cout << "Application shutdown" << std::endl;
}

void Application::Update(float deltaTime) {
    m_InputHandler->Update(deltaTime);

    for (auto& object : m_GeometryObjects) {
        if (object->IsVisible()) {
            object->Update(deltaTime);
        }
    }
}

void Application::Render() {
    m_Renderer->BeginFrame();
    m_Renderer->Clear(glm::vec4(0.95f, 0.95f, 0.95f, 1.0f));

    // Render grid and axes
    if (m_Grid->IsVisible()) {
        m_Grid->Render(m_Renderer->GetLineShader(), m_Camera.get());
    }
    if (m_Axes->IsVisible()) {
        m_Axes->Render(m_Renderer->GetLineShader(), m_Camera.get());
    }

    // Render geometry objects
    for (auto& object : m_GeometryObjects) {
        if (object->IsVisible()) {
            m_Renderer->RenderGeometry(object.get(), m_Camera.get());
        }
    }

    // Render UI
    m_UIManager->BeginFrame();
    m_UIManager->Render();
    m_UIManager->EndFrame();

    m_Renderer->EndFrame();
}

void Application::AddGeometryObject(std::shared_ptr<GeometryObject> object) {
    m_GeometryObjects.push_back(object);
}

void Application::RemoveGeometryObject(std::shared_ptr<GeometryObject> object) {
    auto it = std::find(m_GeometryObjects.begin(), m_GeometryObjects.end(), object);
    if (it != m_GeometryObjects.end()) {
        m_GeometryObjects.erase(it);
    }
}

