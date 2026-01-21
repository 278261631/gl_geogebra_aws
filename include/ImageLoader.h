#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

class ImageLoader {
public:
    ImageLoader();
    ~ImageLoader();

    bool LoadImage(const std::string& filepath);
    void UnloadImage();

    bool IsLoaded() const { return m_Data != nullptr; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    int GetChannels() const { return m_Channels; }

    // Get pixel value at (x, y) - returns grayscale value [0, 255]
    unsigned char GetPixelValue(int x, int y) const;

    // Get RGB color at (x, y) - returns vec3 with values [0.0, 1.0]
    glm::vec3 GetPixelColor(int x, int y) const;

    // Get normalized pixel value [0.0, 1.0]
    float GetNormalizedPixelValue(int x, int y) const;

    // Generate point cloud from image
    // x, y = pixel coordinates, z = pixel value (normalized)
    std::vector<glm::vec3> GeneratePointCloud(float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f) const;

    // Generate point cloud with original colors
    void GeneratePointCloudWithColors(std::vector<glm::vec3>& positions,
                                      std::vector<glm::vec4>& colors,
                                      float scaleX = 1.0f,
                                      float scaleY = 1.0f,
                                      float scaleZ = 1.0f) const;

private:
    unsigned char* m_Data;
    int m_Width;
    int m_Height;
    int m_Channels;
};

