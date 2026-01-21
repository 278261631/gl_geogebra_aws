#pragma once

#include <glm/glm.hpp>

namespace Math {

class Vector3 {
public:
    static float Dot(const glm::vec3& a, const glm::vec3& b);
    static glm::vec3 Cross(const glm::vec3& a, const glm::vec3& b);
    static float Length(const glm::vec3& v);
    static glm::vec3 Normalize(const glm::vec3& v);
    static float Distance(const glm::vec3& a, const glm::vec3& b);
    static glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t);
};

}

