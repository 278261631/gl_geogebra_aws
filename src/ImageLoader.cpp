#define STB_IMAGE_IMPLEMENTATION
#include "ImageLoader.h"
#include "FitsLoader.h"
#include <stb_image.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cmath>

ImageLoader::ImageLoader()
    : m_Data(nullptr)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
    , m_FitsLoader(nullptr)
{
}

ImageLoader::~ImageLoader() {
    UnloadImage();
}

bool ImageLoader::LoadImage(const std::string& filepath) {
    // Check file extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "fits" || ext == "fit" || ext == "fts") {
        return LoadFitsImage(filepath);
    } else {
        return LoadStandardImage(filepath);
    }
}

bool ImageLoader::LoadStandardImage(const std::string& filepath) {
    UnloadImage();

    // Load image with stb_image
    m_Data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_Channels, 0);

    if (!m_Data) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
        return false;
    }

    std::cout << "Image loaded: " << filepath << std::endl;
    std::cout << "  Size: " << m_Width << "x" << m_Height << std::endl;
    std::cout << "  Channels: " << m_Channels << std::endl;

    return true;
}

bool ImageLoader::LoadFitsImage(const std::string& filepath) {
    UnloadImage();

    m_FitsLoader = std::make_unique<FitsLoader>();

    if (!m_FitsLoader->LoadFits(filepath)) {
        m_FitsLoader.reset();
        return false;
    }

    m_Width = m_FitsLoader->GetWidth();
    m_Height = m_FitsLoader->GetHeight();
    m_Channels = 1;  // FITS is grayscale

    return true;
}

void ImageLoader::UnloadImage() {
    if (m_Data) {
        stbi_image_free(m_Data);
        m_Data = nullptr;
    }

    if (m_FitsLoader) {
        m_FitsLoader.reset();
    }

    m_Width = 0;
    m_Height = 0;
    m_Channels = 0;
}

unsigned char ImageLoader::GetPixelValue(int x, int y) const {
    if (m_FitsLoader) {
        // FITS data
        float normalized = m_FitsLoader->GetNormalizedPixelValue(x, y);
        return static_cast<unsigned char>(normalized * 255.0f);
    }

    if (!m_Data || x < 0 || x >= m_Width || y < 0 || y >= m_Height) {
        return 0;
    }

    int index = (y * m_Width + x) * m_Channels;

    // Convert to grayscale if needed
    if (m_Channels == 1) {
        return m_Data[index];
    } else if (m_Channels >= 3) {
        // Use luminance formula: 0.299*R + 0.587*G + 0.114*B
        unsigned char r = m_Data[index];
        unsigned char g = m_Data[index + 1];
        unsigned char b = m_Data[index + 2];
        return static_cast<unsigned char>(0.299f * r + 0.587f * g + 0.114f * b);
    }

    return m_Data[index];
}

float ImageLoader::GetNormalizedPixelValue(int x, int y) const {
    if (m_FitsLoader) {
        return m_FitsLoader->GetNormalizedPixelValue(x, y);
    }
    return GetPixelValue(x, y) / 255.0f;
}

glm::vec3 ImageLoader::GetPixelColor(int x, int y) const {
    if (m_FitsLoader) {
        float r, g, b;
        m_FitsLoader->GetPixelColor(x, y, r, g, b);
        return glm::vec3(r, g, b);
    }

    if (!m_Data || x < 0 || x >= m_Width || y < 0 || y >= m_Height) {
        return glm::vec3(0.0f);
    }

    int index = (y * m_Width + x) * m_Channels;

    if (m_Channels == 1) {
        // Grayscale
        float gray = m_Data[index] / 255.0f;
        return glm::vec3(gray, gray, gray);
    } else if (m_Channels >= 3) {
        // RGB
        float r = m_Data[index] / 255.0f;
        float g = m_Data[index + 1] / 255.0f;
        float b = m_Data[index + 2] / 255.0f;
        return glm::vec3(r, g, b);
    }

    return glm::vec3(0.0f);
}

std::vector<glm::vec3> ImageLoader::GeneratePointCloud(float scaleX, float scaleY, float scaleZ) const {
    std::vector<glm::vec3> points;

    if (!IsLoaded()) {
        return points;
    }

    points.reserve(m_Width * m_Height);

    // Center the point cloud
    float centerX = m_Width * 0.5f;
    float centerZ = m_Height * 0.5f;

    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            float pixelValue = GetNormalizedPixelValue(x, y);

            // Create point: pixel(x,y) -> 3D(x,z), pixel value -> y height
            glm::vec3 point;
            point.x = (x - centerX) * scaleX;           // Pixel X -> World X
            point.y = pixelValue * scaleY;              // Pixel value -> World Y (height)
            point.z = (y - centerZ) * scaleZ;           // Pixel Y -> World Z

            points.push_back(point);
        }
    }

    std::cout << "Generated point cloud with " << points.size() << " points" << std::endl;

    return points;
}

void ImageLoader::GeneratePointCloudWithColors(std::vector<glm::vec3>& positions,
                                               std::vector<glm::vec4>& colors,
                                               float scaleX,
                                               float scaleY,
                                               float scaleZ) const {
    positions.clear();
    colors.clear();

    if (!IsLoaded()) {
        return;
    }

    positions.reserve(m_Width * m_Height);
    colors.reserve(m_Width * m_Height);

    // Center the point cloud
    float centerX = m_Width * 0.5f;
    float centerZ = m_Height * 0.5f;

    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            // Get grayscale value for height
            float pixelValue = GetNormalizedPixelValue(x, y);

            // Get original RGB color
            glm::vec3 rgb = GetPixelColor(x, y);

            // Create point: pixel(x,y) -> 3D(x,z), pixel value -> y height
            glm::vec3 point;
            point.x = (x - centerX) * scaleX;           // Pixel X -> World X
            point.y = pixelValue * scaleY;              // Pixel value -> World Y (height)
            point.z = (y - centerZ) * scaleZ;           // Pixel Y -> World Z

            positions.push_back(point);
            colors.push_back(glm::vec4(rgb, 1.0f));     // Use original RGB color
        }
    }

    std::cout << "Generated point cloud with " << positions.size() << " points (with original colors)" << std::endl;
}

void ImageLoader::GeneratePointCloudWithColorsROI(std::vector<glm::vec3>& positions,
                                                  std::vector<glm::vec4>& colors,
                                                  int pixelX,
                                                  int pixelY,
                                                  int radiusPixels,
                                                  float scaleX,
                                                  float scaleY,
                                                  float scaleZ) const {
    positions.clear();
    colors.clear();

    if (!IsLoaded()) {
        return;
    }

    if (m_Width <= 0 || m_Height <= 0) {
        return;
    }

    if (radiusPixels < 0) radiusPixels = 0;

    // Clamp ROI bounds to image
    const int x0 = std::max(0, pixelX - radiusPixels);
    const int x1 = std::min(m_Width - 1, pixelX + radiusPixels);
    const int y0 = std::max(0, pixelY - radiusPixels);
    const int y1 = std::min(m_Height - 1, pixelY + radiusPixels);

    if (x0 > x1 || y0 > y1) {
        return;
    }

    const std::size_t approxCount =
        static_cast<std::size_t>(x1 - x0 + 1) * static_cast<std::size_t>(y1 - y0 + 1);
    positions.reserve(approxCount);
    colors.reserve(approxCount);

    // Keep the same world-space centering as full image, so ROI aligns with the original image coordinates.
    const float centerX = m_Width * 0.5f;
    const float centerZ = m_Height * 0.5f;

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            // Get grayscale value for height
            const float pixelValue = GetNormalizedPixelValue(x, y);

            // Get original RGB color
            const glm::vec3 rgb = GetPixelColor(x, y);

            // Create point: pixel(x,y) -> 3D(x,z), pixel value -> y height
            glm::vec3 point;
            point.x = (x - centerX) * scaleX;
            point.y = pixelValue * scaleY;
            point.z = (y - centerZ) * scaleZ;

            positions.push_back(point);
            colors.push_back(glm::vec4(rgb, 1.0f));
        }
    }

    std::cout << "Generated ROI point cloud with " << positions.size()
              << " points (pixel center=" << pixelX << "," << pixelY
              << " radius=" << radiusPixels << ")" << std::endl;
}
