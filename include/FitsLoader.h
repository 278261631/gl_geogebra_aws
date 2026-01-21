#pragma once

#include <string>
#include <vector>

// FITS loader using cfitsio library
// Supports 2D image extensions with various data types
class FitsLoader {
public:
    FitsLoader();
    ~FitsLoader();

    // Load FITS file
    bool LoadFits(const std::string& filepath);

    // Get image dimensions
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    int GetBitDepth() const { return m_BitDepth; }

    // Get normalized pixel value [0.0, 1.0]
    float GetNormalizedPixelValue(int x, int y) const;

    // Get RGB color (grayscale for FITS)
    void GetPixelColor(int x, int y, float& r, float& g, float& b) const;

    // Check if loaded
    bool IsLoaded() const { return !m_Data.empty(); }

    // Unload data
    void Unload();

private:
    std::vector<float> m_Data;  // Normalized float data [0.0, 1.0]
    int m_Width;
    int m_Height;
    int m_BitDepth;  // BITPIX value from FITS header

    // Data range for normalization
    float m_MinValue;
    float m_MaxValue;
};

