#define STB_IMAGE_IMPLEMENTATION
#include "ImageLoader.h"
#include <stb_image.h>
#include <iostream>

ImageLoader::ImageLoader()
    : m_Data(nullptr)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
{
}

ImageLoader::~ImageLoader() {
    UnloadImage();
}

bool ImageLoader::LoadImage(const std::string& filepath) {
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

void ImageLoader::UnloadImage() {
    if (m_Data) {
        stbi_image_free(m_Data);
        m_Data = nullptr;
        m_Width = 0;
        m_Height = 0;
        m_Channels = 0;
    }
}

unsigned char ImageLoader::GetPixelValue(int x, int y) const {
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
    return GetPixelValue(x, y) / 255.0f;
}

glm::vec3 ImageLoader::GetPixelColor(int x, int y) const {
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

    if (!m_Data) {
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

    if (!m_Data) {
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

