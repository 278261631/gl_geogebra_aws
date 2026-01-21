#pragma once

#include <memory>

class Shader;
class Camera;

class Axes {
public:
    Axes(float length = 5.0f);
    ~Axes();

    void Initialize();
    void Render(Shader* shader, Camera* camera);

    void SetLength(float length) { m_Length = length; }
    void SetVisible(bool visible) { m_Visible = visible; }

    float GetLength() const { return m_Length; }
    bool IsVisible() const { return m_Visible; }

private:
    void GenerateAxes();

    float m_Length;
    bool m_Visible;

    unsigned int m_VAO;
    unsigned int m_VBO;
};

