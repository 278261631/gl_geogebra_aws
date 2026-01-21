#include "Grid.h"
#include "Shader.h"
#include "Camera.h"
#include <glad/glad.h>
#include <vector>

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
    float halfSize = m_Size / 2.0f;
    float step = m_Size / m_Divisions;

    // Grid lines parallel to X axis
    for (int i = 0; i <= m_Divisions; ++i) {
        float z = -halfSize + i * step;
        float gray = 0.7f;
        
        vertices.push_back(-halfSize); vertices.push_back(0.0f); vertices.push_back(z);
        vertices.push_back(gray); vertices.push_back(gray); vertices.push_back(gray);
        
        vertices.push_back(halfSize); vertices.push_back(0.0f); vertices.push_back(z);
        vertices.push_back(gray); vertices.push_back(gray); vertices.push_back(gray);
    }

    // Grid lines parallel to Z axis
    for (int i = 0; i <= m_Divisions; ++i) {
        float x = -halfSize + i * step;
        float gray = 0.7f;
        
        vertices.push_back(x); vertices.push_back(0.0f); vertices.push_back(-halfSize);
        vertices.push_back(gray); vertices.push_back(gray); vertices.push_back(gray);
        
        vertices.push_back(x); vertices.push_back(0.0f); vertices.push_back(halfSize);
        vertices.push_back(gray); vertices.push_back(gray); vertices.push_back(gray);
    }

    m_VertexCount = vertices.size() / 6;

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

