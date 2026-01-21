#include "Math/Matrix4.h"

namespace Math {

glm::mat4 Matrix4::Identity() {
    return glm::mat4(1.0f);
}

glm::mat4 Matrix4::Translate(const glm::vec3& translation) {
    return glm::translate(glm::mat4(1.0f), translation);
}

glm::mat4 Matrix4::Rotate(float angle, const glm::vec3& axis) {
    return glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}

glm::mat4 Matrix4::Scale(const glm::vec3& scale) {
    return glm::scale(glm::mat4(1.0f), scale);
}

glm::mat4 Matrix4::LookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
    return glm::lookAt(eye, center, up);
}

glm::mat4 Matrix4::Perspective(float fov, float aspect, float near, float far) {
    return glm::perspective(glm::radians(fov), aspect, near, far);
}

glm::mat4 Matrix4::Orthographic(float left, float right, float bottom, float top, float near, float far) {
    return glm::ortho(left, right, bottom, top, near, far);
}

}

