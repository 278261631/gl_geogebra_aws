#pragma once

#include "../GeometryObject.h"

class Sphere : public GeometryObject {
public:
    Sphere(const glm::vec3& center = glm::vec3(0.0f), float radius = 1.0f);
    ~Sphere();

    void Initialize() override;
    void Render(Shader* shader) override;

    void SetRadius(float radius) { m_Radius = radius; }
    float GetRadius() const { return m_Radius; }

private:
    void GenerateSphere();

    float m_Radius;
    int m_Segments;
    int m_Rings;
};

