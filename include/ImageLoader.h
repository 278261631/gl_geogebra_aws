#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class FitsLoader;

class ImageLoader {
public:
    ImageLoader();
    ~ImageLoader();

    bool LoadImage(const std::string& filepath);
    void UnloadImage();

    bool IsLoaded() const { return m_Data != nullptr || m_FitsLoader != nullptr; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    int GetChannels() const { return m_Channels; }
    bool IsFits() const { return m_FitsLoader != nullptr; }

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

    // Generate point cloud with original colors, but recolor pixels in a square highlight region.
    // highlightSizePixels: e.g. 10 means [cx-5..cx+4] × [cy-5..cy+4]
    void GeneratePointCloudWithColorsHighlight(std::vector<glm::vec3>& positions,
                                               std::vector<glm::vec4>& colors,
                                               int highlightCenterX,
                                               int highlightCenterY,
                                               int highlightSizePixels,
                                               const glm::vec4& highlightColor,
                                               float scaleX = 1.0f,
                                               float scaleY = 1.0f,
                                               float scaleZ = 1.0f) const;

    // Generate point cloud with original colors, but only within a pixel ROI around (pixelX, pixelY).
    // radiusPixels: neighborhood radius in pixels (e.g. 50-500).
    void GeneratePointCloudWithColorsROI(std::vector<glm::vec3>& positions,
                                         std::vector<glm::vec4>& colors,
                                         int pixelX,
                                         int pixelY,
                                         int radiusPixels,
                                         float scaleX = 1.0f,
                                         float scaleY = 1.0f,
                                         float scaleZ = 1.0f) const;

    // ROI version with square highlight recolor around (highlightCenterX, highlightCenterY).
    void GeneratePointCloudWithColorsROIHighlight(std::vector<glm::vec3>& positions,
                                                  std::vector<glm::vec4>& colors,
                                                  int pixelX,
                                                  int pixelY,
                                                  int radiusPixels,
                                                  int highlightCenterX,
                                                  int highlightCenterY,
                                                  int highlightSizePixels,
                                                  const glm::vec4& highlightColor,
                                                  float scaleX = 1.0f,
                                                  float scaleY = 1.0f,
                                                  float scaleZ = 1.0f) const;

private:
    bool LoadStandardImage(const std::string& filepath);
    bool LoadFitsImage(const std::string& filepath);

    unsigned char* m_Data;
    int m_Width;
    int m_Height;
    int m_Channels;

    std::unique_ptr<FitsLoader> m_FitsLoader;
};

