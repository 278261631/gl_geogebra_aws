#pragma once

#include "../GeometryObject.h"

class Point : public GeometryObject {
public:
    Point(const glm::vec3& position = glm::vec3(0.0f));
    ~Point();

    void Initialize() override;
    void Render(Shader* shader) override;

    void SetPointSize(float size) { m_PointSize = size; }
    float GetPointSize() const { return m_PointSize; }

private:
    float m_PointSize;
};

