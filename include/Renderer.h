#pragma once

#include <memory>
#include <glm/glm.hpp>

class Camera;
class Shader;
class GeometryObject;

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Initialize();
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    void Clear(const glm::vec4& color = glm::vec4(0.95f, 0.95f, 0.95f, 1.0f));
    void SetViewport(int x, int y, int width, int height);

    void RenderGeometry(GeometryObject* object, Camera* camera);
    void RenderPointCloud(GeometryObject* object, Camera* camera);
    void RenderLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, Camera* camera);
    void RenderGrid(float size, int divisions, Camera* camera);
    void RenderAxes(float length, Camera* camera);

    Shader* GetDefaultShader() { return m_DefaultShader.get(); }
    Shader* GetLineShader() { return m_LineShader.get(); }
    Shader* GetPointCloudShader() { return m_PointCloudShader.get(); }

private:
    void SetupShaders();

    std::unique_ptr<Shader> m_DefaultShader;
    std::unique_ptr<Shader> m_LineShader;
    std::unique_ptr<Shader> m_PointCloudShader;

    unsigned int m_LineVAO;
    unsigned int m_LineVBO;
};

