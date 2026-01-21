#include "Geometry/Sphere.h"
#include "Shader.h"
#include <glad/glad.h>
#include <vector>
#include <cmath>

Sphere::Sphere(const glm::vec3& center, float radius)
    : GeometryObject(GeometryType::Sphere)
    , m_Radius(radius)
    , m_Segments(32)
    , m_Rings(16)
{
    m_Position = center;
}

Sphere::~Sphere() {
}

void Sphere::Initialize() {
    GenerateSphere();
}

void Sphere::GenerateSphere() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265359f;

    for (int ring = 0; ring <= m_Rings; ++ring) {
        float phi = PI * (float)ring / (float)m_Rings;
        
        for (int seg = 0; seg <= m_Segments; ++seg) {
            float theta = 2.0f * PI * (float)seg / (float)m_Segments;

            float x = m_Radius * sin(phi) * cos(theta);
            float y = m_Radius * cos(phi);
            float z = m_Radius * sin(phi) * sin(theta);

            float nx = sin(phi) * cos(theta);
            float ny = cos(phi);
            float nz = sin(phi) * sin(theta);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    for (int ring = 0; ring < m_Rings; ++ring) {
        for (int seg = 0; seg < m_Segments; ++seg) {
            int current = ring * (m_Segments + 1) + seg;
            int next = current + m_Segments + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    m_IndexCount = indices.size();
}

void Sphere::Render(Shader* shader) {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

