#include "Application.h"
#include "GeometryObject.h"
#include "Geometry/Sphere.h"
#include "Geometry/Cube.h"
#include "Geometry/Point.h"
#include "Geometry/PointCloud.h"
#include "UI/FileBrowser.h"
#include "UI/LabelDataBrowser.h"
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>

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

    // Create image loader
    m_ImageLoader = std::make_unique<ImageLoader>();

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

    // Check for new file check/uncheck
    FileBrowser* fileBrowser = m_UIManager->GetFileBrowser();
    if (fileBrowser) {
        // Handle new check
        if (fileBrowser->HasNewCheck()) {
            std::string checkedFile = fileBrowser->GetNewCheckedFile();
            fileBrowser->ClearNewCheckFlag();

            std::cout << "Loading image: " << checkedFile << std::endl;
            LoadImageAndGeneratePoints(checkedFile);
        }

        // Handle new uncheck
        if (fileBrowser->HasNewUncheck()) {
            std::string uncheckedFile = fileBrowser->GetNewUncheckedFile();
            fileBrowser->ClearNewUncheckFlag();

            std::cout << "Removing image points: " << uncheckedFile << std::endl;
            RemoveImagePoints(uncheckedFile);
        }
    }

    // Check for new FITS pair selection from label txt
    LabelDataBrowser* labelBrowser = m_UIManager->GetLabelDataBrowser();
    if (labelBrowser && labelBrowser->HasNewFitsPair()) {
        const std::string alignedFits = labelBrowser->GetNewAlignedFitsPath();
        const std::string templateFits = labelBrowser->GetNewTemplateFitsPath();
        const std::string sourceTxt = labelBrowser->GetNewFitsSourceTxtPath();
        labelBrowser->ClearNewFitsPairFlag();

        namespace fs = std::filesystem;

        std::cout << "TXT selected: " << sourceTxt << std::endl;
        std::cout << "Loading FITS pair:" << std::endl;
        std::cout << "  aligned:  " << alignedFits << std::endl;
        std::cout << "  template: " << templateFits << std::endl;

        if (!alignedFits.empty() && fs::exists(fs::path(alignedFits))) {
            LoadImageAndGeneratePoints(alignedFits);
        } else {
            std::cerr << "Aligned FITS not found: " << alignedFits << std::endl;
        }

        if (!templateFits.empty() && fs::exists(fs::path(templateFits))) {
            LoadImageAndGeneratePoints(templateFits);
        } else {
            std::cerr << "Template FITS not found: " << templateFits << std::endl;
        }
    }

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
            // Use optimized point cloud renderer for PointCloud objects
            if (object->GetType() == GeometryType::Point &&
                dynamic_cast<PointCloud*>(object.get()) != nullptr) {
                m_Renderer->RenderPointCloud(object.get(), m_Camera.get());
            } else {
                m_Renderer->RenderGeometry(object.get(), m_Camera.get());
            }
        }
    }

    // Render UI
    m_UIManager->BeginFrame();
    m_UIManager->Render();

    // Render 3D labels for grid and axes
    Render3DLabels();

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

void Application::LoadImageAndGeneratePoints(const std::string& filepath) {
    // Check if already loaded
    if (m_ImagePointsMap.find(filepath) != m_ImagePointsMap.end()) {
        std::cout << "Image already loaded: " << filepath << std::endl;
        return;
    }

    if (!m_ImageLoader->LoadImage(filepath)) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
        return;
    }

    // Generate point cloud from image with original colors
    // Now: pixel(x,y) -> 3D(x,z), pixel value -> y height
    float scaleX = 0.1f;  // Scale for X (pixel X -> world X)
    float scaleY = 10.0f; // Scale for Y (pixel value -> world Y height)
    float scaleZ = 0.1f;  // Scale for Z (pixel Y -> world Z)

    std::vector<glm::vec3> positions;
    std::vector<glm::vec4> colors;

    // Use the new method that preserves original colors
    m_ImageLoader->GeneratePointCloudWithColors(positions, colors, scaleX, scaleY, scaleZ);

    std::cout << "Creating point cloud with " << positions.size() << " points..." << std::endl;

    // Calculate image size in world coordinates
    float imageWidth = m_ImageLoader->GetWidth() * scaleX;
    float imageDepth = m_ImageLoader->GetHeight() * scaleZ;
    float imageDiagonal = std::sqrt(imageWidth * imageWidth + imageDepth * imageDepth);

    // Auto-frame the camera to view the entire image
    // Only adjust if this is the first image or if the image is larger than current view
    if (m_ImagePointsMap.empty() || imageDiagonal > 50.0f) {
        m_Camera->FrameView(imageDiagonal);
        std::cout << "Camera adjusted to frame image (size: " << imageDiagonal << " units)" << std::endl;
    }

    // Create a single PointCloud object for all points
    auto pointCloud = std::make_shared<PointCloud>();
    pointCloud->SetPointData(positions, colors);
    pointCloud->SetPointSize(3.0f);
    pointCloud->SetName("PointCloud_" + filepath);
    pointCloud->Initialize();

    AddGeometryObject(pointCloud);

    // Store the point cloud associated with this image
    std::vector<std::shared_ptr<GeometryObject>> imageObjects;
    imageObjects.push_back(pointCloud);
    m_ImagePointsMap[filepath] = imageObjects;

    std::cout << "Point cloud created with " << positions.size() << " points (1 draw call)" << std::endl;
}

void Application::RemoveImagePoints(const std::string& filepath) {
    auto it = m_ImagePointsMap.find(filepath);
    if (it == m_ImagePointsMap.end()) {
        std::cout << "No points found for image: " << filepath << std::endl;
        return;
    }

    // Remove all points associated with this image
    for (auto& point : it->second) {
        RemoveGeometryObject(point);
    }

    std::cout << "Removed " << it->second.size() << " points for image: " << filepath << std::endl;

    // Remove from map
    m_ImagePointsMap.erase(it);
}

void Application::Render3DLabels() {
    if (!m_Camera || !m_Window) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix();
    glm::mat4 viewProj = projection * view;

    int windowWidth = m_Window->GetWidth();
    int windowHeight = m_Window->GetHeight();

    auto worldToScreen = [&](const glm::vec3& worldPos) -> glm::vec2 {
        glm::vec4 clipSpace = viewProj * glm::vec4(worldPos, 1.0f);
        if (clipSpace.w <= 0.0f) return glm::vec2(-1000, -1000); // Behind camera

        glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;

        float screenX = (ndc.x + 1.0f) * 0.5f * windowWidth;
        float screenY = (1.0f - ndc.y) * 0.5f * windowHeight;

        return glm::vec2(screenX, screenY);
    };

    // Render axis labels
    if (m_Axes && m_Axes->IsVisible()) {
        float length = m_Axes->GetLength();

        // X axis label (red)
        glm::vec2 xPos = worldToScreen(glm::vec3(length + 0.5f, 0, 0));
        if (xPos.x >= 0 && xPos.x < windowWidth && xPos.y >= 0 && xPos.y < windowHeight) {
            drawList->AddText(ImVec2(xPos.x, xPos.y), IM_COL32(255, 0, 0, 255), "X");
        }

        // Y axis label (green)
        glm::vec2 yPos = worldToScreen(glm::vec3(0, length + 0.5f, 0));
        if (yPos.x >= 0 && yPos.x < windowWidth && yPos.y >= 0 && yPos.y < windowHeight) {
            drawList->AddText(ImVec2(yPos.x, yPos.y), IM_COL32(0, 255, 0, 255), "Y");
        }

        // Z axis label (blue)
        glm::vec2 zPos = worldToScreen(glm::vec3(0, 0, length + 0.5f));
        if (zPos.x >= 0 && zPos.x < windowWidth && zPos.y >= 0 && zPos.y < windowHeight) {
            drawList->AddText(ImVec2(zPos.x, zPos.y), IM_COL32(0, 0, 255, 255), "Z");
        }
    }

    // Render grid labels
    if (m_Grid && m_Grid->IsVisible()) {
        const auto& labels = m_Grid->GetLabels();
        for (const auto& label : labels) {
            glm::vec2 screenPos = worldToScreen(label.position);
            if (screenPos.x >= 0 && screenPos.x < windowWidth &&
                screenPos.y >= 0 && screenPos.y < windowHeight) {
                drawList->AddText(ImVec2(screenPos.x, screenPos.y),
                                IM_COL32(100, 100, 100, 200),
                                label.text.c_str());
            }
        }
    }
}

