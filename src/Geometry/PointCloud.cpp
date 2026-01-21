#include "Geometry/PointCloud.h"
#include "Shader.h"
#include <glad/glad.h>
#include <iostream>

PointCloud::PointCloud()
    : GeometryObject(GeometryType::Point)
    , m_PointSize(5.0f)
    , m_PointCount(0)
    , m_NeedsUpdate(false)
{
}

PointCloud::~PointCloud() {
}

void PointCloud::Initialize() {
    if (m_Positions.empty()) {
        return;
    }

    // Create interleaved vertex data: position (3) + color (4)
    std::vector<float> vertices;
    vertices.reserve(m_Positions.size() * 7);

    for (size_t i = 0; i < m_Positions.size(); i++) {
        // Position
        vertices.push_back(m_Positions[i].x);
        vertices.push_back(m_Positions[i].y);
        vertices.push_back(m_Positions[i].z);
        
        // Color
        vertices.push_back(m_Colors[i].r);
        vertices.push_back(m_Colors[i].g);
        vertices.push_back(m_Colors[i].b);
        vertices.push_back(m_Colors[i].a);
    }

    m_PointCount = static_cast<int>(m_Positions.size());

    // Clean up old buffers if they exist
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);

    // Create VAO and VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    m_VertexCount = m_PointCount;
    m_NeedsUpdate = false;

    std::cout << "PointCloud initialized with " << m_PointCount << " points" << std::endl;
}

void PointCloud::Update(float deltaTime) {
    if (m_NeedsUpdate) {
        UpdateBuffers();
    }
}

void PointCloud::Render(Shader* shader) {
    if (m_PointCount == 0) return;

    glPointSize(m_PointSize);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, m_PointCount);
    glBindVertexArray(0);
    glPointSize(1.0f);
}

void PointCloud::SetPointData(const std::vector<glm::vec3>& positions, 
                               const std::vector<glm::vec4>& colors) {
    m_Positions = positions;
    m_Colors = colors;
    
    // Ensure colors match positions
    if (m_Colors.size() < m_Positions.size()) {
        m_Colors.resize(m_Positions.size(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    
    m_NeedsUpdate = true;
}

void PointCloud::UpdateBuffers() {
    if (m_Positions.empty()) {
        return;
    }

    // Update existing buffer data
    std::vector<float> vertices;
    vertices.reserve(m_Positions.size() * 7);

    for (size_t i = 0; i < m_Positions.size(); i++) {
        vertices.push_back(m_Positions[i].x);
        vertices.push_back(m_Positions[i].y);
        vertices.push_back(m_Positions[i].z);
        vertices.push_back(m_Colors[i].r);
        vertices.push_back(m_Colors[i].g);
        vertices.push_back(m_Colors[i].b);
        vertices.push_back(m_Colors[i].a);
    }

    m_PointCount = static_cast<int>(m_Positions.size());

    if (m_VBO != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    m_VertexCount = m_PointCount;
    m_NeedsUpdate = false;
}

