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
#include <algorithm>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>

Application::Application()
    : m_Running(false)
    , m_LastFrameTime(0.0f)
    , m_HasFramedView(false)
    , m_AlignedPreviewTex(0)
    , m_TemplatePreviewTex(0)
    , m_AlignedPreviewSize(0)
    , m_TemplatePreviewSize(0)
    , m_AlignedPreviewCenterX(0)
    , m_AlignedPreviewCenterY(0)
    , m_TemplatePreviewCenterX(0)
    , m_TemplatePreviewCenterY(0)
    , m_HasAlignedPreviewClick(false)
    , m_HasTemplatePreviewClick(false)
    , m_AlignedPreviewClickX(0)
    , m_AlignedPreviewClickY(0)
    , m_TemplatePreviewClickX(0)
    , m_TemplatePreviewClickY(0)
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
    // Delete preview textures while OpenGL context is still valid.
    if (m_AlignedPreviewTex != 0) {
        glDeleteTextures(1, &m_AlignedPreviewTex);
        m_AlignedPreviewTex = 0;
    }
    if (m_TemplatePreviewTex != 0) {
        glDeleteTextures(1, &m_TemplatePreviewTex);
        m_TemplatePreviewTex = 0;
    }
    m_AlignedPreviewSize = 0;
    m_TemplatePreviewSize = 0;
    m_AlignedPreviewName.clear();
    m_TemplatePreviewName.clear();

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
    if (labelBrowser && labelBrowser->HasCenterCameraOnRoiRequest()) {
        labelBrowser->ClearCenterCameraOnRoiRequest();

        if (labelBrowser->HasActivePixelCenter()) {
            const int roiX = labelBrowser->GetActivePixelX();
            const int roiY = labelBrowser->GetActivePixelY();

            // Prefer aligned FITS if present, otherwise template FITS.
            std::string fitsPath = labelBrowser->GetNewAlignedFitsPath();
            if (fitsPath.empty()) {
                fitsPath = labelBrowser->GetNewTemplateFitsPath();
            }

            namespace fs = std::filesystem;
            if (!fitsPath.empty() && fs::exists(fs::path(fitsPath))) {
                // Load image data so we can sample the ROI center height.
                if (m_ImageLoader->LoadImage(fitsPath)) {
                    // Must match the mapping used in LoadImageAndGeneratePointsInternal.
                    const float scaleX = 0.1f;
                    const float scaleY = 10.0f;
                    const float scaleZ = 0.1f;

                    const float centerX = m_ImageLoader->GetWidth() * 0.5f;
                    const float centerZ = m_ImageLoader->GetHeight() * 0.5f;

                    const float height = m_ImageLoader->GetNormalizedPixelValue(roiX, roiY) * scaleY;
                    const glm::vec3 newTarget(
                        (roiX - centerX) * scaleX,
                        height,
                        (roiY - centerZ) * scaleZ);

                    // Keep view direction stable: translate position by the same delta as target.
                    const glm::vec3 oldTarget = m_Camera->GetTarget();
                    const glm::vec3 delta = newTarget - oldTarget;
                    m_Camera->SetTarget(newTarget);
                    m_Camera->SetPosition(m_Camera->GetPosition() + delta);
                }
            }
        }
    }

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

        const bool useRoi = labelBrowser->HasActivePixelCenter() && labelBrowser->IsRoiEnabled();
        const int roiX = labelBrowser->HasActivePixelCenter() ? labelBrowser->GetActivePixelX() : labelBrowser->GetPixelX();
        const int roiY = labelBrowser->HasActivePixelCenter() ? labelBrowser->GetActivePixelY() : labelBrowser->GetPixelY();
        int roiR = labelBrowser->GetRoiRadius();
        if (roiR < 50) roiR = 50;
        if (roiR > 500) roiR = 500;

        // Auto center camera on ROI center when switching targets, but keep current zoom (distance).
        // We load one of the FITS once to compute the world-space Y at (roiX, roiY).
        if (labelBrowser->HasActivePixelCenter()) {
            std::string centerFits = alignedFits;
            if (centerFits.empty() || !fs::exists(fs::path(centerFits))) {
                centerFits = templateFits;
            }
            if (!centerFits.empty() && fs::exists(fs::path(centerFits))) {
                if (m_ImageLoader->LoadImage(centerFits)) {
                    CenterCameraOnPixelInCurrentImage(roiX, roiY);
                }
            }
        }

        if (!alignedFits.empty() && fs::exists(fs::path(alignedFits))) {
            // Highlight 10x10 around ROI center:
            // aligned -> orange-red, template -> sky-blue
            const bool doHighlight = labelBrowser->HasActivePixelCenter();
            int highlightSize = labelBrowser->GetHighlightSizePixels();
            if (highlightSize < 1) highlightSize = 1;
            if (highlightSize > 300) highlightSize = 300;
            float highlightScale = labelBrowser->GetHighlightPointSizeScale();
            if (highlightScale < 1.0f) highlightScale = 1.0f;
            if (highlightScale > 20.0f) highlightScale = 20.0f;
            const glm::vec4 alignedHighlight(1.0f, 0.2706f, 0.0f, 1.0f);   // OrangeRed (#FF4500)
            LoadImageAndGeneratePointsInternal(alignedFits, /*replaceExisting*/ true, useRoi, roiX, roiY, roiR,
                                               doHighlight, roiX, roiY, highlightSize, alignedHighlight, highlightScale,
                                               /*previewSlot*/ 1);
        } else {
            std::cerr << "Aligned FITS not found: " << alignedFits << std::endl;
        }

        if (!templateFits.empty() && fs::exists(fs::path(templateFits))) {
            const bool doHighlight = labelBrowser->HasActivePixelCenter();
            int highlightSize = labelBrowser->GetHighlightSizePixels();
            if (highlightSize < 1) highlightSize = 1;
            if (highlightSize > 300) highlightSize = 300;
            float highlightScale = labelBrowser->GetHighlightPointSizeScale();
            if (highlightScale < 1.0f) highlightScale = 1.0f;
            if (highlightScale > 20.0f) highlightScale = 20.0f;
            const glm::vec4 templateHighlight(0.5294f, 0.8078f, 0.9216f, 1.0f); // SkyBlue (#87CEEB)
            LoadImageAndGeneratePointsInternal(templateFits, /*replaceExisting*/ true, useRoi, roiX, roiY, roiR,
                                               doHighlight, roiX, roiY, highlightSize, templateHighlight, highlightScale,
                                               /*previewSlot*/ 2);
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

void Application::CenterCameraOnPixelInCurrentImage(int pixelX, int pixelY) {
    if (!m_Camera || !m_ImageLoader || !m_ImageLoader->IsLoaded()) return;

    // Must match the mapping used in LoadImageAndGeneratePointsInternal.
    const float scaleX = 0.1f;
    const float scaleY = 10.0f;
    const float scaleZ = 0.1f;

    const float centerX = m_ImageLoader->GetWidth() * 0.5f;
    const float centerZ = m_ImageLoader->GetHeight() * 0.5f;

    const float height = m_ImageLoader->GetNormalizedPixelValue(pixelX, pixelY) * scaleY;
    const glm::vec3 newTarget(
        (pixelX - centerX) * scaleX,
        height,
        (pixelY - centerZ) * scaleZ);

    // Keep view direction / zoom stable: translate position by the same delta as target.
    const glm::vec3 oldTarget = m_Camera->GetTarget();
    const glm::vec3 delta = newTarget - oldTarget;
    m_Camera->SetTarget(newTarget);
    m_Camera->SetPosition(m_Camera->GetPosition() + delta);
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

    // Top-right preview for aligned/template ROI patch
    RenderFitsRoiPreviewWindow();

    // Render 3D labels for grid and axes
    Render3DLabels();

    m_UIManager->EndFrame();

    m_Renderer->EndFrame();
}

static void EnsureTexture2D(unsigned int& tex) {
    if (tex != 0) return;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Application::UpdatePreviewTextureFromCurrentImage(int previewSlot,
                                                       const std::string& filepath,
                                                       int cropCenterX,
                                                       int cropCenterY,
                                                       int cropSizePixels,
                                                       const glm::vec4& tintColor,
                                                       float tintAlpha) {
    if (previewSlot != 1 && previewSlot != 2) return;
    if (!m_ImageLoader || !m_ImageLoader->IsLoaded()) return;

    cropSizePixels = std::clamp(cropSizePixels, 1, 300);
    // NOTE: Preview should show original FITS brightness (no tint). We keep the params for API stability.
    (void)tintColor;
    (void)tintAlpha;

    unsigned int* texPtr = (previewSlot == 1) ? &m_AlignedPreviewTex : &m_TemplatePreviewTex;
    int* sizePtr = (previewSlot == 1) ? &m_AlignedPreviewSize : &m_TemplatePreviewSize;
    std::string* namePtr = (previewSlot == 1) ? &m_AlignedPreviewName : &m_TemplatePreviewName;
    int* centerXPtr = (previewSlot == 1) ? &m_AlignedPreviewCenterX : &m_TemplatePreviewCenterX;
    int* centerYPtr = (previewSlot == 1) ? &m_AlignedPreviewCenterY : &m_TemplatePreviewCenterY;

    EnsureTexture2D(*texPtr);
    *sizePtr = cropSizePixels;
    *centerXPtr = cropCenterX;
    *centerYPtr = cropCenterY;
    try {
        *namePtr = std::filesystem::path(filepath).filename().string();
    } catch (...) {
        *namePtr = filepath;
    }

    const int w = m_ImageLoader->GetWidth();
    const int h = m_ImageLoader->GetHeight();
    if (w <= 0 || h <= 0) return;

    const int half = cropSizePixels / 2;
    const int xStart = cropCenterX - half;
    const int yStart = cropCenterY - half;

    const bool isFits = m_ImageLoader->IsFits();
    std::vector<unsigned char> rgba(static_cast<std::size_t>(cropSizePixels) * cropSizePixels * 4);

    // For FITS, do a local contrast stretch so deep bit-depth data looks like a normal image.
    float stretchLow = 0.0f;
    float stretchHigh = 1.0f;
    std::vector<float> grayVals;
    if (isFits) {
        grayVals.reserve(static_cast<std::size_t>(cropSizePixels) * cropSizePixels);
        for (int j = 0; j < cropSizePixels; j++) {
            for (int i = 0; i < cropSizePixels; i++) {
                const int x = std::clamp(xStart + i, 0, w - 1);
                const int y = std::clamp(yStart + j, 0, h - 1);
                grayVals.push_back(m_ImageLoader->GetNormalizedPixelValue(x, y));
            }
        }
        auto quantile = [&](float q) -> float {
            if (grayVals.empty()) return 0.0f;
            const std::size_t n = grayVals.size();
            const std::size_t idx = static_cast<std::size_t>(std::clamp(q, 0.0f, 1.0f) * float(n - 1));
            std::vector<float> tmp = grayVals;
            std::nth_element(tmp.begin(), tmp.begin() + idx, tmp.end());
            return tmp[idx];
        };
        stretchLow = quantile(0.01f);
        stretchHigh = quantile(0.99f);
        if (stretchHigh - stretchLow < 1e-6f) {
            stretchLow = 0.0f;
            stretchHigh = 1.0f;
        }
    }

    for (int j = 0; j < cropSizePixels; j++) {
        for (int i = 0; i < cropSizePixels; i++) {
            const int x = std::clamp(xStart + i, 0, w - 1);
            const int y = std::clamp(yStart + j, 0, h - 1);

            glm::vec3 out(0.0f);
            if (isFits) {
                // Contrast stretch to [0,1], then apply a mild gamma to lift shadows.
                const float v = m_ImageLoader->GetNormalizedPixelValue(x, y);
                float t = (v - stretchLow) / (stretchHigh - stretchLow);
                t = std::clamp(t, 0.0f, 1.0f);
                // Gamma-like curve (sqrt) to make dim structures more visible.
                t = std::sqrt(t);
                out = glm::vec3(t, t, t);
            } else {
                // Standard images are already in display range.
                out = m_ImageLoader->GetPixelColor(x, y);
            }

            const std::size_t idx = (static_cast<std::size_t>(j) * cropSizePixels + i) * 4;
            rgba[idx + 0] = static_cast<unsigned char>(std::clamp(out.r, 0.0f, 1.0f) * 255.0f);
            rgba[idx + 1] = static_cast<unsigned char>(std::clamp(out.g, 0.0f, 1.0f) * 255.0f);
            rgba[idx + 2] = static_cast<unsigned char>(std::clamp(out.b, 0.0f, 1.0f) * 255.0f);
            rgba[idx + 3] = 255;
        }
    }

    glBindTexture(GL_TEXTURE_2D, *texPtr);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, cropSizePixels, cropSizePixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Application::RenderFitsRoiPreviewWindow() {
    if (m_AlignedPreviewTex == 0 && m_TemplatePreviewTex == 0) return;

    const int windowWidth = m_Window ? m_Window->GetWidth() : 0;
    const int windowHeight = m_Window ? m_Window->GetHeight() : 0;
    if (windowWidth <= 0 || windowHeight <= 0) return;

    const float margin = 10.0f;
    const float preferredWinW = 320.0f;

    ImGui::SetNextWindowPos(ImVec2(windowWidth - preferredWinW - margin, margin), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.85f);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize;

    if (!ImGui::Begin("FITS ROI Preview", nullptr, flags)) {
        ImGui::End();
        return;
    }

    const float targetImageW = 280.0f; // keep aspect correct and stable

    auto drawCrosshairHollow = [&](ImDrawList* dl, const ImVec2& center, ImU32 col) {
        const float len = 16.0f;
        const float gap = 4.0f;
        const float thick = 2.0f;
        // Horizontal segments
        dl->AddLine(ImVec2(center.x - len, center.y), ImVec2(center.x - gap, center.y), col, thick);
        dl->AddLine(ImVec2(center.x + gap, center.y), ImVec2(center.x + len, center.y), col, thick);
        // Vertical segments
        dl->AddLine(ImVec2(center.x, center.y - len), ImVec2(center.x, center.y - gap), col, thick);
        dl->AddLine(ImVec2(center.x, center.y + gap), ImVec2(center.x, center.y + len), col, thick);
    };

    auto drawPreview = [&](int previewSlot, const char* label, unsigned int tex, const std::string& name) {
        ImGui::Text("%s: %s", label, name.empty() ? "(none)" : name.c_str());
        if (tex != 0) {
            // Keep aspect ratio correct (our preview textures are square crops).
            ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(tex)), ImVec2(targetImageW, targetImageW));

            // Handle click -> update ROI center and request reload/center camera.
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && m_UIManager && m_UIManager->GetLabelDataBrowser()) {
                LabelDataBrowser* lb = m_UIManager->GetLabelDataBrowser();
                const ImVec2 p0 = ImGui::GetItemRectMin();
                const ImVec2 p1 = ImGui::GetItemRectMax();
                const ImVec2 mp = ImGui::GetIO().MousePos;

                const float u = (p1.x > p0.x) ? (mp.x - p0.x) / (p1.x - p0.x) : 0.0f;
                const float v = (p1.y > p0.y) ? (mp.y - p0.y) / (p1.y - p0.y) : 0.0f;

                int cropSize = (previewSlot == 1) ? m_AlignedPreviewSize : m_TemplatePreviewSize;
                int cropCenterX = (previewSlot == 1) ? m_AlignedPreviewCenterX : m_TemplatePreviewCenterX;
                int cropCenterY = (previewSlot == 1) ? m_AlignedPreviewCenterY : m_TemplatePreviewCenterY;
                cropSize = std::max(1, cropSize);

                const int half = cropSize / 2;
                const int xStart = cropCenterX - half;
                const int yStart = cropCenterY - half;

                const int clickX = xStart + static_cast<int>(std::clamp(u, 0.0f, 0.999999f) * cropSize);
                const int clickY = yStart + static_cast<int>(std::clamp(v, 0.0f, 0.999999f) * cropSize);

                // Save for crosshair rendering (in pixel coordinates)
                if (previewSlot == 1) {
                    m_HasAlignedPreviewClick = true;
                    m_AlignedPreviewClickX = clickX;
                    m_AlignedPreviewClickY = clickY;
                } else {
                    m_HasTemplatePreviewClick = true;
                    m_TemplatePreviewClickX = clickX;
                    m_TemplatePreviewClickY = clickY;
                }

                lb->SetActivePixelCenter(clickX, clickY);
            }

            // Draw crosshair overlay at last clicked pixel
            {
                const bool hasClick = (previewSlot == 1) ? m_HasAlignedPreviewClick : m_HasTemplatePreviewClick;
                if (hasClick) {
                    const int cropSize = (previewSlot == 1) ? m_AlignedPreviewSize : m_TemplatePreviewSize;
                    const int cropCenterX = (previewSlot == 1) ? m_AlignedPreviewCenterX : m_TemplatePreviewCenterX;
                    const int cropCenterY = (previewSlot == 1) ? m_AlignedPreviewCenterY : m_TemplatePreviewCenterY;
                    const int clickX = (previewSlot == 1) ? m_AlignedPreviewClickX : m_TemplatePreviewClickX;
                    const int clickY = (previewSlot == 1) ? m_AlignedPreviewClickY : m_TemplatePreviewClickY;

                    const int half = std::max(1, cropSize) / 2;
                    const int xStart = cropCenterX - half;
                    const int yStart = cropCenterY - half;

                    const float u = (static_cast<float>(clickX - xStart) + 0.5f) / static_cast<float>(std::max(1, cropSize));
                    const float v = (static_cast<float>(clickY - yStart) + 0.5f) / static_cast<float>(std::max(1, cropSize));

                    const ImVec2 p0 = ImGui::GetItemRectMin();
                    const ImVec2 p1 = ImGui::GetItemRectMax();
                    const ImVec2 center(p0.x + std::clamp(u, 0.0f, 1.0f) * (p1.x - p0.x),
                                        p0.y + std::clamp(v, 0.0f, 1.0f) * (p1.y - p0.y));

                    ImDrawList* dl = ImGui::GetWindowDrawList();
                    const ImU32 col = (previewSlot == 1)
                        ? IM_COL32(255, 69, 0, 255)   // aligned: OrangeRed
                        : IM_COL32(30, 144, 255, 255); // template: DodgerBlue
                    drawCrosshairHollow(dl, center, col);
                }
            }
        } else {
            ImGui::Dummy(ImVec2(targetImageW, targetImageW));
            ImGui::TextUnformatted("(no preview)");
        }
    };

    drawPreview(1, "aligned", m_AlignedPreviewTex, m_AlignedPreviewName);
    ImGui::Separator();
    drawPreview(2, "template", m_TemplatePreviewTex, m_TemplatePreviewName);

    ImGui::End();
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
    // Normal image loading: keep previous behavior (skip if already loaded).
    LoadImageAndGeneratePointsInternal(filepath, /*replaceExisting*/ false, /*useRoi*/ false, 0, 0, 0,
                                       /*useHighlight*/ false, 0, 0, 0, glm::vec4(0.0f), 1.0f,
                                       /*previewSlot*/ 0);
}

void Application::LoadImageAndGeneratePointsInternal(const std::string& filepath,
                                                     bool replaceExisting,
                                                     bool useRoi,
                                                     int roiPixelX,
                                                     int roiPixelY,
                                                     int roiRadiusPixels,
                                                     bool useHighlight,
                                                     int highlightCenterX,
                                                     int highlightCenterY,
                                                     int highlightSizePixels,
                                                     const glm::vec4& highlightColor,
                                                     float highlightPointSizeScale,
                                                     int previewSlot) {
    if (!replaceExisting) {
        // Check if already loaded
        if (m_ImagePointsMap.find(filepath) != m_ImagePointsMap.end()) {
            std::cout << "Image already loaded: " << filepath << std::endl;
            return;
        }
    } else {
        // Replace existing point cloud(s) for this filepath
        if (m_ImagePointsMap.find(filepath) != m_ImagePointsMap.end()) {
            RemoveImagePoints(filepath);
        }
    }

    if (!m_ImageLoader->LoadImage(filepath)) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
        return;
    }

    // Update preview texture using the freshly loaded image.
    if (previewSlot == 1 || previewSlot == 2) {
        // Use highlightSizePixels as crop size (i.e., "染色区域") and tint it.
        // We keep a moderate alpha so the underlying content remains visible.
        // Preview should be raw FITS brightness (no tint). Stretch is handled internally.
        UpdatePreviewTextureFromCurrentImage(previewSlot, filepath, highlightCenterX, highlightCenterY, highlightSizePixels, highlightColor, 0.0f);
    }

    // Generate point cloud from image with original colors
    // Now: pixel(x,y) -> 3D(x,z), pixel value -> y height
    const float scaleX = 0.1f;  // Scale for X (pixel X -> world X)
    const float scaleY = 10.0f; // Scale for Y (pixel value -> world Y height)
    const float scaleZ = 0.1f;  // Scale for Z (pixel Y -> world Z)

    std::vector<glm::vec3> positions;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec3> highlightPositions;
    std::vector<glm::vec4> highlightColors;

    // For highlight rendering, we split highlight points into a separate buffer so we can render them larger.
    if (useHighlight) {
        if (highlightPointSizeScale < 1.0f) highlightPointSizeScale = 1.0f;
        if (highlightPointSizeScale > 20.0f) highlightPointSizeScale = 20.0f;

        int effectiveRadius = roiRadiusPixels;
        if (!useRoi) {
            // Use a large ROI to cover the entire image, but still allow highlight split logic.
            effectiveRadius = std::max(m_ImageLoader->GetWidth(), m_ImageLoader->GetHeight());
        }

        m_ImageLoader->GeneratePointCloudWithColorsROISplitHighlight(
            positions, colors,
            highlightPositions, highlightColors,
            roiPixelX, roiPixelY, effectiveRadius,
            highlightCenterX, highlightCenterY, highlightSizePixels, highlightColor,
            scaleX, scaleY, scaleZ);
    } else {
        if (useRoi) {
            m_ImageLoader->GeneratePointCloudWithColorsROI(positions, colors, roiPixelX, roiPixelY, roiRadiusPixels, scaleX, scaleY, scaleZ);
        } else {
            m_ImageLoader->GeneratePointCloudWithColors(positions, colors, scaleX, scaleY, scaleZ);
        }
    }

    std::cout << "Creating point cloud with " << positions.size() << " points..." << std::endl;

    // Calculate image size in world coordinates
    float imageWidth = m_ImageLoader->GetWidth() * scaleX;
    float imageDepth = m_ImageLoader->GetHeight() * scaleZ;
    float imageDiagonal = std::sqrt(imageWidth * imageWidth + imageDepth * imageDepth);

    // Auto-frame camera only once (initial experience). Do NOT re-frame on target switches, so zoom stays stable.
    if (!m_HasFramedView && !replaceExisting) {
        m_Camera->FrameView(imageDiagonal);
        m_HasFramedView = true;
        std::cout << "Camera adjusted to frame image (size: " << imageDiagonal << " units)" << std::endl;
    }

    // Create point cloud(s)
    const float basePointSize = 3.0f;

    auto pointCloud = std::make_shared<PointCloud>();
    pointCloud->SetPointData(positions, colors);
    pointCloud->SetPointSize(basePointSize);
    pointCloud->SetName("PointCloud_" + filepath);
    pointCloud->Initialize();

    AddGeometryObject(pointCloud);

    // Store the point cloud associated with this image
    std::vector<std::shared_ptr<GeometryObject>> imageObjects;
    imageObjects.push_back(pointCloud);

    if (useHighlight && !highlightPositions.empty()) {
        auto highlightCloud = std::make_shared<PointCloud>();
        highlightCloud->SetPointData(highlightPositions, highlightColors);
        highlightCloud->SetPointSize(basePointSize * highlightPointSizeScale);
        highlightCloud->SetName("PointCloud_" + filepath + "_highlight");
        highlightCloud->Initialize();
        AddGeometryObject(highlightCloud);
        imageObjects.push_back(highlightCloud);
    }

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

