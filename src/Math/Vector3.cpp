#include "Math/Vector3.h"

namespace Math {

float Vector3::Dot(const glm::vec3& a, const glm::vec3& b) {
    return glm::dot(a, b);
}

glm::vec3 Vector3::Cross(const glm::vec3& a, const glm::vec3& b) {
    return glm::cross(a, b);
}

float Vector3::Length(const glm::vec3& v) {
    return glm::length(v);
}

glm::vec3 Vector3::Normalize(const glm::vec3& v) {
    return glm::normalize(v);
}

float Vector3::Distance(const glm::vec3& a, const glm::vec3& b) {
    return glm::distance(a, b);
}

glm::vec3 Vector3::Lerp(const glm::vec3& a, const glm::vec3& b, float t) {
    return glm::mix(a, b, t);
}

}

