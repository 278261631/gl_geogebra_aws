#include "Geometry/Line.h"
#include "Shader.h"
#include <glad/glad.h>

Line::Line(const glm::vec3& start, const glm::vec3& end)
    : GeometryObject(GeometryType::Line)
    , m_Start(start)
    , m_End(end)
{
}

Line::~Line() {
}

void Line::Initialize() {
    float vertices[] = {
        m_Start.x, m_Start.y, m_Start.z,  0.0f, 0.0f, 1.0f,
        m_End.x, m_End.y, m_End.z,  0.0f, 0.0f, 1.0f
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

    m_VertexCount = 2;
}

void Line::Render(Shader* shader) {
    glLineWidth(2.0f);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINES, 0, m_VertexCount);
    glBindVertexArray(0);
    glLineWidth(1.0f);
}

