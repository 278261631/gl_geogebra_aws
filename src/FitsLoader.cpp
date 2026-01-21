#include "FitsLoader.h"
#include <fitsio.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

FitsLoader::FitsLoader()
    : m_Width(0)
    , m_Height(0)
    , m_BitDepth(0)
    , m_MinValue(0.0f)
    , m_MaxValue(1.0f)
{
}

FitsLoader::~FitsLoader() {
    Unload();
}

void FitsLoader::Unload() {
    m_Data.clear();
    m_Width = 0;
    m_Height = 0;
    m_BitDepth = 0;
}

bool FitsLoader::LoadFits(const std::string& filepath) {
    Unload();
    
    fitsfile* fptr = nullptr;
    int status = 0;
    
    std::cout << "Loading FITS file: " << filepath << std::endl;
    
    // Open FITS file
    if (fits_open_file(&fptr, filepath.c_str(), READONLY, &status)) {
        char err_text[FLEN_STATUS];
        fits_get_errstatus(status, err_text);
        std::cerr << "Failed to open FITS file: " << err_text << std::endl;
        return false;
    }
    
    // Get image dimensions
    int naxis = 0;
    long naxes[2] = {0, 0};
    
    if (fits_get_img_dim(fptr, &naxis, &status)) {
        char err_text[FLEN_STATUS];
        fits_get_errstatus(status, err_text);
        std::cerr << "Failed to get image dimensions: " << err_text << std::endl;
        fits_close_file(fptr, &status);
        return false;
    }
    
    if (naxis != 2) {
        std::cerr << "Only 2D FITS images are supported (NAXIS=" << naxis << ")" << std::endl;
        fits_close_file(fptr, &status);
        return false;
    }
    
    if (fits_get_img_size(fptr, 2, naxes, &status)) {
        char err_text[FLEN_STATUS];
        fits_get_errstatus(status, err_text);
        std::cerr << "Failed to get image size: " << err_text << std::endl;
        fits_close_file(fptr, &status);
        return false;
    }
    
    m_Width = static_cast<int>(naxes[0]);
    m_Height = static_cast<int>(naxes[1]);
    
    // Get BITPIX value
    int bitpix = 0;
    if (fits_get_img_type(fptr, &bitpix, &status)) {
        char err_text[FLEN_STATUS];
        fits_get_errstatus(status, err_text);
        std::cerr << "Failed to get BITPIX: " << err_text << std::endl;
        fits_close_file(fptr, &status);
        return false;
    }
    
    m_BitDepth = bitpix;
    
    std::cout << "  BITPIX: " << m_BitDepth << std::endl;
    std::cout << "  NAXIS: " << naxis << std::endl;
    std::cout << "  Size: " << m_Width << "x" << m_Height << std::endl;
    
    // Read image data as float
    int numPixels = m_Width * m_Height;
    m_Data.resize(numPixels);
    
    long fpixel[2] = {1, 1};  // Start from first pixel
    float nullval = 0.0f;
    int anynull = 0;
    
    if (fits_read_pix(fptr, TFLOAT, fpixel, numPixels, &nullval, 
                      m_Data.data(), &anynull, &status)) {
        char err_text[FLEN_STATUS];
        fits_get_errstatus(status, err_text);
        std::cerr << "Failed to read image data: " << err_text << std::endl;
        fits_close_file(fptr, &status);
        return false;
    }
    
    // Close FITS file
    fits_close_file(fptr, &status);
    
    // Find min and max values for normalization
    m_MinValue = std::numeric_limits<float>::max();
    m_MaxValue = std::numeric_limits<float>::lowest();
    
    for (float val : m_Data) {
        if (std::isfinite(val)) {
            if (val < m_MinValue) m_MinValue = val;
            if (val > m_MaxValue) m_MaxValue = val;
        }
    }
    
    std::cout << "  Data range: [" << m_MinValue << ", " << m_MaxValue << "]" << std::endl;
    
    // Normalize to [0, 1]
    float range = m_MaxValue - m_MinValue;
    if (range > 0.0f) {
        for (float& val : m_Data) {
            if (std::isfinite(val)) {
                val = (val - m_MinValue) / range;
            } else {
                val = 0.0f;
            }
        }
    }
    
    std::cout << "FITS file loaded successfully" << std::endl;
    return true;
}

float FitsLoader::GetNormalizedPixelValue(int x, int y) const {
    if (!IsLoaded() || x < 0 || x >= m_Width || y < 0 || y >= m_Height) {
        return 0.0f;
    }
    
    int index = y * m_Width + x;
    return m_Data[index];
}

void FitsLoader::GetPixelColor(int x, int y, float& r, float& g, float& b) const {
    float gray = GetNormalizedPixelValue(x, y);
    r = g = b = gray;
}

