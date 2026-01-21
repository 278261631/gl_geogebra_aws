#pragma once

#include "GeometryObject.h"
#include <vector>
#include <glm/glm.hpp>

class PointCloud : public GeometryObject {
public:
    PointCloud();
    ~PointCloud() override;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(Shader* shader) override;

    // Set point cloud data
    void SetPointData(const std::vector<glm::vec3>& positions, 
                      const std::vector<glm::vec4>& colors);
    
    void SetPointSize(float size) { m_PointSize = size; }
    float GetPointSize() const { return m_PointSize; }
    
    int GetPointCount() const { return m_PointCount; }

private:
    void UpdateBuffers();

    std::vector<glm::vec3> m_Positions;
    std::vector<glm::vec4> m_Colors;
    
    float m_PointSize;
    int m_PointCount;
    bool m_NeedsUpdate;
};

