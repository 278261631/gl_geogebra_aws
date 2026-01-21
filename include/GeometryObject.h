#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>

class Shader;

enum class GeometryType {
    Point,
    Line,
    Plane,
    Sphere,
    Cube,
    Cylinder,
    Cone
};

class GeometryObject {
public:
    GeometryObject(GeometryType type);
    virtual ~GeometryObject();

    virtual void Initialize() = 0;
    virtual void Render(Shader* shader) = 0;
    virtual void Update(float deltaTime) {}

    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
    void SetScale(const glm::vec3& scale) { m_Scale = scale; }
    void SetColor(const glm::vec4& color) { m_Color = color; }
    void SetVisible(bool visible) { m_Visible = visible; }
    void SetName(const std::string& name) { m_Name = name; }

    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetRotation() const { return m_Rotation; }
    glm::vec3 GetScale() const { return m_Scale; }
    glm::vec4 GetColor() const { return m_Color; }
    bool IsVisible() const { return m_Visible; }
    std::string GetName() const { return m_Name; }
    GeometryType GetType() const { return m_Type; }

    glm::mat4 GetModelMatrix() const;

protected:
    GeometryType m_Type;
    glm::vec3 m_Position;
    glm::vec3 m_Rotation;
    glm::vec3 m_Scale;
    glm::vec4 m_Color;
    bool m_Visible;
    std::string m_Name;

    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;
    unsigned int m_VertexCount;
    unsigned int m_IndexCount;
};

