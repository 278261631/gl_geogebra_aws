#include "Geometry/Point.h"
#include "Shader.h"
#include <glad/glad.h>

Point::Point(const glm::vec3& position)
    : GeometryObject(GeometryType::Point)
    , m_PointSize(10.0f)
{
    m_Position = position;
}

Point::~Point() {
}

void Point::Initialize() {
    float vertices[] = {
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    m_VertexCount = 1;
}

void Point::Render(Shader* shader) {
    glPointSize(m_PointSize);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, m_VertexCount);
    glBindVertexArray(0);
    glPointSize(1.0f);
}

