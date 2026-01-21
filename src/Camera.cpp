#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
    : m_Position(0.0f, 5.0f, 10.0f)
    , m_Target(0.0f, 0.0f, 0.0f)
    , m_Up(0.0f, 1.0f, 0.0f)
    , m_FOV(fov)
    , m_AspectRatio(aspectRatio)
    , m_NearPlane(nearPlane)
    , m_FarPlane(farPlane)
    , m_Distance(10.0f)
    , m_Yaw(0.0f)
    , m_Pitch(30.0f)
{
    UpdateProjectionMatrix();
}

void Camera::SetPosition(const glm::vec3& position) {
    m_Position = position;
    m_Distance = glm::length(m_Position - m_Target);
}

void Camera::SetTarget(const glm::vec3& target) {
    m_Target = target;
    m_Distance = glm::length(m_Position - m_Target);
}

void Camera::SetUp(const glm::vec3& up) {
    m_Up = up;
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_Position, m_Target, m_Up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return m_ProjectionMatrix;
}

void Camera::UpdateProjectionMatrix() {
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::Rotate(float yaw, float pitch) {
    m_Yaw += yaw;
    m_Pitch += pitch;

    // Clamp pitch
    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;

    // Calculate new position
    glm::vec3 direction;
    direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    direction.y = sin(glm::radians(m_Pitch));
    direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Position = m_Target - glm::normalize(direction) * m_Distance;
}

void Camera::Zoom(float delta) {
    // Exponential zoom: multiply distance by a factor
    // This provides smooth, intuitive zooming at any distance
    float zoomFactor = 1.0f - delta * 0.1f;  // 10% per scroll step
    m_Distance *= zoomFactor;

    // Clamp distance to reasonable range
    // Increased max distance to support large images (e.g., 3000x3000)
    if (m_Distance < 0.5f) m_Distance = 0.5f;
    if (m_Distance > 1000.0f) m_Distance = 1000.0f;  // Increased from 200 to 1000

    glm::vec3 direction = glm::normalize(m_Position - m_Target);
    m_Position = m_Target + direction * m_Distance;
}

void Camera::Pan(float deltaX, float deltaY) {
    glm::vec3 right = GetRight();
    glm::vec3 up = m_Up;

    m_Position += right * deltaX + up * deltaY;
    m_Target += right * deltaX + up * deltaY;
}

void Camera::Orbit(float deltaX, float deltaY) {
    m_Yaw += deltaX;
    m_Pitch += deltaY;

    // Clamp pitch
    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;

    // Calculate new position
    float x = m_Distance * cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
    float y = m_Distance * sin(glm::radians(m_Pitch));
    float z = m_Distance * cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));

    m_Position = m_Target + glm::vec3(x, y, z);
}

void Camera::FrameView(float objectSize) {
    // Calculate optimal distance to frame an object of given size
    // Using FOV to determine how far back the camera needs to be
    float halfFOV = glm::radians(m_FOV * 0.5f);
    float distance = (objectSize * 0.5f) / tan(halfFOV);

    // Add some padding (1.5x)
    distance *= 1.5f;

    // Clamp to valid range
    if (distance < 0.5f) distance = 0.5f;
    if (distance > 1000.0f) distance = 1000.0f;

    m_Distance = distance;

    // Update position based on current angles
    float x = m_Distance * cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
    float y = m_Distance * sin(glm::radians(m_Pitch));
    float z = m_Distance * cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));

    m_Position = m_Target + glm::vec3(x, y, z);
}

