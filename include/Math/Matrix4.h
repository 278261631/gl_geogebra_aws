#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Math {

class Matrix4 {
public:
    static glm::mat4 Identity();
    static glm::mat4 Translate(const glm::vec3& translation);
    static glm::mat4 Rotate(float angle, const glm::vec3& axis);
    static glm::mat4 Scale(const glm::vec3& scale);
    static glm::mat4 LookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
    static glm::mat4 Perspective(float fov, float aspect, float near, float far);
    static glm::mat4 Orthographic(float left, float right, float bottom, float top, float near, float far);
};

}

