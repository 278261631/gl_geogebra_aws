#pragma once

#include "../GeometryObject.h"

class Plane : public GeometryObject {
public:
    Plane(const glm::vec3& normal = glm::vec3(0.0f, 1.0f, 0.0f), float distance = 0.0f);
    ~Plane();

    void Initialize() override;
    void Render(Shader* shader) override;

    void SetNormal(const glm::vec3& normal) { m_Normal = normal; }
    void SetDistance(float distance) { m_Distance = distance; }
    glm::vec3 GetNormal() const { return m_Normal; }
    float GetDistance() const { return m_Distance; }

private:
    glm::vec3 m_Normal;
    float m_Distance;
    float m_Size;
};

