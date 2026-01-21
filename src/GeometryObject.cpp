#include "GeometryObject.h"
#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

GeometryObject::GeometryObject(GeometryType type)
    : m_Type(type)
    , m_Position(0.0f)
    , m_Rotation(0.0f)
    , m_Scale(1.0f)
    , m_Color(1.0f)
    , m_Visible(true)
    , m_Name("Object")
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_VertexCount(0)
    , m_IndexCount(0)
{
}

GeometryObject::~GeometryObject() {
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
    if (m_EBO != 0) glDeleteBuffers(1, &m_EBO);
}

glm::mat4 GeometryObject::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
    model = glm::rotate(model, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, m_Scale);
    return model;
}

