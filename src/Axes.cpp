#include "Axes.h"
#include "Shader.h"
#include "Camera.h"
#include <glad/glad.h>

Axes::Axes(float length)
    : m_Length(length)
    , m_Visible(true)
    , m_VAO(0)
    , m_VBO(0)
{
}

Axes::~Axes() {
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
}

void Axes::Initialize() {
    GenerateAxes();
}

void Axes::GenerateAxes() {
    float vertices[] = {
        // X axis (red)
        0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        m_Length, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        
        // Y axis (green)
        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f, m_Length, 0.0f,  0.0f, 1.0f, 0.0f,
        
        // Z axis (blue)
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, m_Length,  0.0f, 0.0f, 1.0f
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
}

void Axes::Render(Shader* shader, Camera* camera) {
    if (!m_Visible) return;

    shader->Use();
    shader->SetMat4("view", camera->GetViewMatrix());
    shader->SetMat4("projection", camera->GetProjectionMatrix());

    glLineWidth(3.0f);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
    glLineWidth(1.0f);
}

