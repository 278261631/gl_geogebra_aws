#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);

    void SetPosition(const glm::vec3& position);
    void SetTarget(const glm::vec3& target);
    void SetUp(const glm::vec3& up);

    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetTarget() const { return m_Target; }
    glm::vec3 GetUp() const { return m_Up; }
    glm::vec3 GetForward() const { return glm::normalize(m_Target - m_Position); }
    glm::vec3 GetRight() const { return glm::normalize(glm::cross(GetForward(), m_Up)); }

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    void SetFOV(float fov) { m_FOV = fov; UpdateProjectionMatrix(); }
    void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; UpdateProjectionMatrix(); }

    void Rotate(float yaw, float pitch);
    void Zoom(float delta);
    void Pan(float deltaX, float deltaY);
    void Orbit(float deltaX, float deltaY);

private:
    void UpdateProjectionMatrix();

    glm::vec3 m_Position;
    glm::vec3 m_Target;
    glm::vec3 m_Up;

    float m_FOV;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    float m_Distance;
    float m_Yaw;
    float m_Pitch;

    glm::mat4 m_ProjectionMatrix;
};

