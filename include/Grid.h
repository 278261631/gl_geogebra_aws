#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

class Shader;
class Camera;

class Grid {
public:
    Grid(float size = 20.0f, int divisions = 20);
    ~Grid();

    void Initialize();
    void Render(Shader* shader, Camera* camera);
    void RenderLabels(Camera* camera);

    void SetSize(float size) { m_Size = size; Initialize(); }
    void SetDivisions(int divisions) { m_Divisions = divisions; Initialize(); }
    void SetVisible(bool visible) { m_Visible = visible; }

    float GetSize() const { return m_Size; }
    int GetDivisions() const { return m_Divisions; }
    bool IsVisible() const { return m_Visible; }

    struct GridLabel {
        glm::vec3 position;
        std::string text;
    };

    const std::vector<GridLabel>& GetLabels() const { return m_Labels; }

private:
    void GenerateGrid();

    float m_Size;
    int m_Divisions;
    bool m_Visible;

    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_VertexCount;

    std::vector<GridLabel> m_Labels;
};

