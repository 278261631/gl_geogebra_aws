#include "Axes.h"
#include "Shader.h"
#include "Camera.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Axes::Axes(float length)
    : m_Length(length)
    , m_Visible(true)
    , m_VAO(0)
    , m_VBO(0)
    , m_VertexCount(0)
{
}

Axes::~Axes() {
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
}

void Axes::Initialize() {
    GenerateAxes();
}

void Axes::GenerateArrow(std::vector<float>& vertices,
                         const glm::vec3& start,
                         const glm::vec3& end,
                         const glm::vec3& color) {
    // Main line
    vertices.push_back(start.x); vertices.push_back(start.y); vertices.push_back(start.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    vertices.push_back(end.x); vertices.push_back(end.y); vertices.push_back(end.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);

    // Arrow head parameters
    float arrowSize = m_Length * 0.1f;
    glm::vec3 direction = glm::normalize(end - start);
    glm::vec3 arrowBase = end - direction * arrowSize;

    // Find perpendicular vectors
    glm::vec3 perpendicular1, perpendicular2;
    if (std::abs(direction.x) < 0.9f) {
        perpendicular1 = glm::normalize(glm::cross(direction, glm::vec3(1, 0, 0)));
    } else {
        perpendicular1 = glm::normalize(glm::cross(direction, glm::vec3(0, 1, 0)));
    }
    perpendicular2 = glm::normalize(glm::cross(direction, perpendicular1));

    // Create arrow cone with 8 lines
    int segments = 8;
    float radius = arrowSize * 0.3f;
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 2.0f * 3.14159f;
        float angle2 = (float)(i + 1) / segments * 2.0f * 3.14159f;

        glm::vec3 p1 = arrowBase + perpendicular1 * std::cos(angle1) * radius + perpendicular2 * std::sin(angle1) * radius;
        glm::vec3 p2 = arrowBase + perpendicular1 * std::cos(angle2) * radius + perpendicular2 * std::sin(angle2) * radius;

        // Line from cone base to tip
        vertices.push_back(p1.x); vertices.push_back(p1.y); vertices.push_back(p1.z);
        vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
        vertices.push_back(end.x); vertices.push_back(end.y); vertices.push_back(end.z);
        vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);

        // Line around cone base
        vertices.push_back(p1.x); vertices.push_back(p1.y); vertices.push_back(p1.z);
        vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
        vertices.push_back(p2.x); vertices.push_back(p2.y); vertices.push_back(p2.z);
        vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    }
}

void Axes::GenerateAxes() {
    std::vector<float> vertices;

    // X axis (red) - pointing right
    GenerateArrow(vertices, glm::vec3(0, 0, 0), glm::vec3(m_Length, 0, 0), glm::vec3(1, 0, 0));

    // Y axis (green) - pointing up
    GenerateArrow(vertices, glm::vec3(0, 0, 0), glm::vec3(0, m_Length, 0), glm::vec3(0, 1, 0));

    // Z axis (blue) - pointing forward
    GenerateArrow(vertices, glm::vec3(0, 0, 0), glm::vec3(0, 0, m_Length), glm::vec3(0, 0, 1));

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

void Axes::Render(Shader* shader, Camera* camera) {
    if (!m_Visible) return;

    shader->Use();
    shader->SetMat4("view", camera->GetViewMatrix());
    shader->SetMat4("projection", camera->GetProjectionMatrix());

    glLineWidth(2.0f);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINES, 0, m_VertexCount);
    glBindVertexArray(0);
    glLineWidth(1.0f);
}

void Axes::RenderLabels(Camera* camera) {
    // This will be implemented with ImGui text rendering
    // For now, it's a placeholder
}

