#pragma once

#include "../GeometryObject.h"

class Line : public GeometryObject {
public:
    Line(const glm::vec3& start = glm::vec3(0.0f), const glm::vec3& end = glm::vec3(1.0f, 0.0f, 0.0f));
    ~Line();

    void Initialize() override;
    void Render(Shader* shader) override;

    void SetStart(const glm::vec3& start) { m_Start = start; }
    void SetEnd(const glm::vec3& end) { m_End = end; }
    glm::vec3 GetStart() const { return m_Start; }
    glm::vec3 GetEnd() const { return m_End; }

private:
    glm::vec3 m_Start;
    glm::vec3 m_End;
};

