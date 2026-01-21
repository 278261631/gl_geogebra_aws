#include "Geometry/Plane.h"
#include "Shader.h"
#include <glad/glad.h>
#include <vector>

Plane::Plane(const glm::vec3& normal, float distance)
    : GeometryObject(GeometryType::Plane)
    , m_Normal(normal)
    , m_Distance(distance)
    , m_Size(5.0f)
{
}

Plane::~Plane() {
}

void Plane::Initialize() {
    std::vector<float> vertices = {
        -m_Size, 0.0f, -m_Size,  0.0f, 1.0f, 0.0f,
         m_Size, 0.0f, -m_Size,  0.0f, 1.0f, 0.0f,
         m_Size, 0.0f,  m_Size,  0.0f, 1.0f, 0.0f,
        -m_Size, 0.0f,  m_Size,  0.0f, 1.0f, 0.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

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

void Plane::Render(Shader* shader) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

