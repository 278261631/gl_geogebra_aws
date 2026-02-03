#pragma once

#include <memory>
#include <map>
#include "Window.h"
#include "Renderer.h"
#include "Camera.h"
#include "InputHandler.h"
#include "UI/UIManager.h"
#include "Grid.h"
#include "Axes.h"
#include "ImageLoader.h"
#include <vector>
#include <string>

class GeometryObject;

class Application {
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

    Window* GetWindow() { return m_Window.get(); }
    Camera* GetCamera() { return m_Camera.get(); }
    Renderer* GetRenderer() { return m_Renderer.get(); }

    void AddGeometryObject(std::shared_ptr<GeometryObject> object);
    void RemoveGeometryObject(std::shared_ptr<GeometryObject> object);
    std::vector<std::shared_ptr<GeometryObject>>& GetGeometryObjects() { return m_GeometryObjects; }

    void LoadImageAndGeneratePoints(const std::string& filepath);
    void RemoveImagePoints(const std::string& filepath);
    void Render3DLabels();

private:
    void Update(float deltaTime);
    void Render();
    void RenderFitsRoiPreviewWindow();

    void LoadImageAndGeneratePointsInternal(const std::string& filepath,
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
                                            int previewSlot /*0:none, 1:aligned, 2:template*/);

    void UpdatePreviewTextureFromCurrentImage(int previewSlot,
                                              const std::string& filepath,
                                              int cropCenterX,
                                              int cropCenterY,
                                              int cropSizePixels,
                                              const glm::vec4& tintColor,
                                              float tintAlpha);

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<InputHandler> m_InputHandler;
    std::unique_ptr<UIManager> m_UIManager;
    std::unique_ptr<Grid> m_Grid;
    std::unique_ptr<Axes> m_Axes;
    std::unique_ptr<ImageLoader> m_ImageLoader;

    std::vector<std::shared_ptr<GeometryObject>> m_GeometryObjects;
    std::map<std::string, std::vector<std::shared_ptr<GeometryObject>>> m_ImagePointsMap;

    bool m_Running;
    float m_LastFrameTime;

    // ROI preview textures (aligned/template)
    unsigned int m_AlignedPreviewTex;
    unsigned int m_TemplatePreviewTex;
    int m_AlignedPreviewSize;
    int m_TemplatePreviewSize;
    std::string m_AlignedPreviewName;
    std::string m_TemplatePreviewName;
};

