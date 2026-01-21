#include "Geometry/Cube.h"
#include "Shader.h"
#include <glad/glad.h>

Cube::Cube(const glm::vec3& center, float size)
    : GeometryObject(GeometryType::Cube)
    , m_Size(size)
{
    m_Position = center;
}

Cube::~Cube() {
}

void Cube::Initialize() {
    GenerateCube();
}

void Cube::GenerateCube() {
    float s = m_Size / 2.0f;
    
    float vertices[] = {
        // Front face
        -s, -s,  s,  0.0f,  0.0f,  1.0f,
         s, -s,  s,  0.0f,  0.0f,  1.0f,
         s,  s,  s,  0.0f,  0.0f,  1.0f,
        -s,  s,  s,  0.0f,  0.0f,  1.0f,
        
        // Back face
        -s, -s, -s,  0.0f,  0.0f, -1.0f,
        -s,  s, -s,  0.0f,  0.0f, -1.0f,
         s,  s, -s,  0.0f,  0.0f, -1.0f,
         s, -s, -s,  0.0f,  0.0f, -1.0f,
        
        // Top face
        -s,  s, -s,  0.0f,  1.0f,  0.0f,
        -s,  s,  s,  0.0f,  1.0f,  0.0f,
         s,  s,  s,  0.0f,  1.0f,  0.0f,
         s,  s, -s,  0.0f,  1.0f,  0.0f,
        
        // Bottom face
        -s, -s, -s,  0.0f, -1.0f,  0.0f,
         s, -s, -s,  0.0f, -1.0f,  0.0f,
         s, -s,  s,  0.0f, -1.0f,  0.0f,
        -s, -s,  s,  0.0f, -1.0f,  0.0f,
        
        // Right face
         s, -s, -s,  1.0f,  0.0f,  0.0f,
         s,  s, -s,  1.0f,  0.0f,  0.0f,
         s,  s,  s,  1.0f,  0.0f,  0.0f,
         s, -s,  s,  1.0f,  0.0f,  0.0f,
        
        // Left face
        -s, -s, -s, -1.0f,  0.0f,  0.0f,
        -s, -s,  s, -1.0f,  0.0f,  0.0f,
        -s,  s,  s, -1.0f,  0.0f,  0.0f,
        -s,  s, -s, -1.0f,  0.0f,  0.0f
    };

    unsigned int indices[] = {
        0,  1,  2,   2,  3,  0,   // Front
        4,  5,  6,   6,  7,  4,   // Back
        8,  9, 10,  10, 11,  8,   // Top
        12, 13, 14,  14, 15, 12,  // Bottom
        16, 17, 18,  18, 19, 16,  // Right
        20, 21, 22,  22, 23, 20   // Left
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    m_IndexCount = 36;
}

void Cube::Render(Shader* shader) {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

