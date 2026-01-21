#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

class Shader;
class Camera;

class Axes {
public:
    Axes(float length = 5.0f);
    ~Axes();

    void Initialize();
    void Render(Shader* shader, Camera* camera);
    void RenderLabels(Camera* camera);

    void SetLength(float length) { m_Length = length; Initialize(); }
    void SetVisible(bool visible) { m_Visible = visible; }

    float GetLength() const { return m_Length; }
    bool IsVisible() const { return m_Visible; }

private:
    void GenerateAxes();
    void GenerateArrow(std::vector<float>& vertices,
                       const glm::vec3& start,
                       const glm::vec3& end,
                       const glm::vec3& color);

    float m_Length;
    bool m_Visible;

    unsigned int m_VAO;
    unsigned int m_VBO;
    int m_VertexCount;
};

