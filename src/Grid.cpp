#include "Grid.h"
#include "Shader.h"
#include "Camera.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

Grid::Grid(float size, int divisions)
    : m_Size(size)
    , m_Divisions(divisions)
    , m_Visible(true)
    , m_VAO(0)
    , m_VBO(0)
    , m_VertexCount(0)
{
}

Grid::~Grid() {
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
}

void Grid::Initialize() {
    GenerateGrid();
}

void Grid::GenerateGrid() {
    std::vector<float> vertices;
    m_Labels.clear();

    float halfSize = m_Size / 2.0f;
    float step = m_Size / m_Divisions;

    // Grid lines parallel to X axis (along Z direction)
    for (int i = 0; i <= m_Divisions; ++i) {
        float z = -halfSize + i * step;
        float gray = (i == m_Divisions / 2) ? 0.5f : 0.7f; // Darker for center line

        vertices.push_back(-halfSize); vertices.push_back(0.0f); vertices.push_back(z);
        vertices.push_back(gray); vertices.push_back(gray); vertices.push_back(gray);

        vertices.push_back(halfSize); vertices.push_back(0.0f); vertices.push_back(z);
        vertices.push_back(gray); vertices.push_back(gray); vertices.push_back(gray);

        // Add label every few divisions
        if (i % 2 == 0 || i == 0 || i == m_Divisions) {
            GridLabel label;
            label.position = glm::vec3(-halfSize - 0.5f, 0.0f, z);
            label.text = std::to_string((int)(z));
            m_Labels.push_back(label);
        }
    }

    // Grid lines parallel to Z axis (along X direction)
    for (int i = 0; i <= m_Divisions; ++i) {
        float x = -halfSize + i * step;
        float gray = (i == m_Divisions / 2) ? 0.5f : 0.7f; // Darker for center line

        vertices.push_back(x); vertices.push_back(0.0f); vertices.push_back(-halfSize);
        vertices.push_back(gray); vertices.push_back(gray); vertices.push_back(gray);

        vertices.push_back(x); vertices.push_back(0.0f); vertices.push_back(halfSize);
        vertices.push_back(gray); vertices.push_back(gray); vertices.push_back(gray);

        // Add label every few divisions
        if (i % 2 == 0 || i == 0 || i == m_Divisions) {
            GridLabel label;
            label.position = glm::vec3(x, 0.0f, -halfSize - 0.5f);
            label.text = std::to_string((int)(x));
            m_Labels.push_back(label);
        }
    }

    m_VertexCount = vertices.size() / 6;

    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Grid::Render(Shader* shader, Camera* camera) {
    if (!m_Visible) return;

    shader->Use();
    shader->SetMat4("view", camera->GetViewMatrix());
    shader->SetMat4("projection", camera->GetProjectionMatrix());

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINES, 0, m_VertexCount);
    glBindVertexArray(0);
}

void Grid::RenderLabels(Camera* camera) {
    // This will be implemented with ImGui text rendering
    // For now, it's a placeholder
}

