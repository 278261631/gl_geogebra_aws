#pragma once

#include "../GeometryObject.h"

class Cube : public GeometryObject {
public:
    Cube(const glm::vec3& center = glm::vec3(0.0f), float size = 1.0f);
    ~Cube();

    void Initialize() override;
    void Render(Shader* shader) override;

    void SetSize(float size) { m_Size = size; }
    float GetSize() const { return m_Size; }

private:
    void GenerateCube();

    float m_Size;
};

