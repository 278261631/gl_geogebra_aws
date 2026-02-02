#include "Geometry/PointCloud.h"
#include "Shader.h"
#include <glad/glad.h>
#include <algorithm>
#include <cstdint>
#include <iostream>

namespace {
struct PackedVertex {
    float x, y, z;
    std::uint8_t r, g, b, a;
};

static std::uint8_t ToU8(float v) {
    v = std::clamp(v, 0.0f, 1.0f);
    return static_cast<std::uint8_t>(v * 255.0f + 0.5f);
}
} // namespace

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

    // Create interleaved vertex data:
    // position: 3 * float (12B)
    // color:    4 * uint8 normalized (4B)
    // Total ~16B/point (vs previous 28B/point with vec4 floats)
    std::vector<PackedVertex> vertices;
    vertices.reserve(m_Positions.size());

    for (size_t i = 0; i < m_Positions.size(); i++) {
        const auto& p = m_Positions[i];
        const auto& c = m_Colors[i];
        vertices.push_back(PackedVertex{
            p.x, p.y, p.z,
            ToU8(c.r), ToU8(c.g), ToU8(c.b), ToU8(c.a),
        });
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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PackedVertex), vertices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PackedVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(PackedVertex), (void*)(3 * sizeof(float)));
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
    std::vector<PackedVertex> vertices;
    vertices.reserve(m_Positions.size());

    for (size_t i = 0; i < m_Positions.size(); i++) {
        const auto& p = m_Positions[i];
        const auto& c = m_Colors[i];
        vertices.push_back(PackedVertex{
            p.x, p.y, p.z,
            ToU8(c.r), ToU8(c.g), ToU8(c.b), ToU8(c.a),
        });
    }

    m_PointCount = static_cast<int>(m_Positions.size());

    if (m_VBO != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PackedVertex), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    m_VertexCount = m_PointCount;
    m_NeedsUpdate = false;
}

