
#include <cstdio>
#include <cmath>
#include "PixelImage.h"
#include <iostream>

PixelImage::PixelImage(unsigned int width, unsigned int height):
m_width(width),
m_height(height)
{
    size_t channelAllocation = m_width * m_height * sizeof(PixelType);

    m_channels = (PixelType**)malloc(sizeof(PixelType*)* NUM_CHANNELS);
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        m_channels[i] = (PixelType*)malloc(channelAllocation);
    }
}

PixelImage::~PixelImage()
{
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        free(m_channels[i]);
    }
    free(m_channels);
}

void PixelImage::SetPixelChannelValue(unsigned int x, unsigned int y, unsigned int channelNumber, PixelType value)
{
    m_channels[channelNumber][y * m_width + x] = value;
}

PixelType PixelImage::GetPixelChannelValue(unsigned int x, unsigned int y, unsigned int channelNumber) const
{
    return m_channels[channelNumber][y * m_width + x];
}

PixelImage* PixelImage::GetResized(unsigned int newWidth, unsigned int newHeight) const
{
    PixelImage* resized = new PixelImage(newWidth, newHeight);

    double dNewWidth = (double)newWidth;
    double dNewHeight = (double)newHeight;
    double dOldWidth = (double)m_width;
    double dOldHeight = (double)m_height;

    double newToOldX = dOldWidth / dNewWidth;
    double newToOldY = dOldHeight / dNewHeight;

    for (unsigned int newY = 0; newY < newHeight; ++newY)
    {
        for (unsigned int newX = 0; newX < newWidth; ++newX)
        {
            // Find the pixel's bounding box on the old image
            // Measure from center of new pixel.
            double leftEdge = (newX) * newToOldX;
            double rightEdge = (newX + 1.0) * newToOldX;
            double topEdge = (newY) * newToOldY;
            double bottomEdge = (newY + 1.0) * newToOldY;

            double area = (rightEdge - leftEdge) * (bottomEdge - topEdge);

            unsigned int minPixelX = (unsigned int)floor(leftEdge);
            unsigned int minPixelY = (unsigned int)floor(topEdge);
            unsigned int maxPixelX = (unsigned int)ceil(rightEdge);
            unsigned int maxPixelY = (unsigned int)ceil(bottomEdge);

            double sums[NUM_CHANNELS];
            for (unsigned int c = 0; c < NUM_CHANNELS; ++c)
            {
                sums[c] = 0.0;
            }

            // Add each pixel value to the "sums" accumulation, weighted by how much
            // of the pixel is inside the "new" pixel bounds.
            for (unsigned int oldY = minPixelY; oldY <= maxPixelY; ++oldY)
            {
                for (unsigned int oldX = minPixelX; oldX <= maxPixelX; ++oldX)
                {
                    double left = fmax((double)oldX, leftEdge);
                    double right = fmin((double)oldX + 1, rightEdge);
                    double top = fmax((double)oldY, topEdge);
                    double bottom = fmin((double)oldY + 1, bottomEdge);

                    double weight = ((right - left) * (bottom - top)) / area;

                    for (unsigned int c = 0; c < NUM_CHANNELS; ++c)
                    {
                        sums[c] += GetPixelChannelValue(oldX, oldY, c) * weight;
                    }
                }
            }

            for (unsigned int c = 0; c < NUM_CHANNELS; ++c)
            {
                resized->SetPixelChannelValue(newX, newY, c, (PixelType)sums[c]);
            }
        }
    }

    return resized;
}

PixelImage* PixelImage::FastMipResize() const
{
    // ensure dimensions are even... or one.
    if ((m_width > 1) && ((m_width & 1) != 0))
    {
        std::cerr << "Attempted to call FastMipResize() on image that didn't have even (or one) width!  Aborting..." << std::endl;
        exit(1);
    }

    if ((m_height > 1) && ((m_height & 1) != 0))
    {
        std::cerr << "Attempted to call FastMipResize() on image that didn't have even (or one) height!  Aborting..." << std::endl;
        exit(1);
    }

    if (m_width == 1)
    {
        if (m_height == 1)
        {
            // already minimum size, return nothing.
            return NULL;
        }

        // Minimum width... only resize height
        unsigned int newWidth = 1;
        unsigned int newHeight = m_height / 2;
        PixelImage* resized = new PixelImage(newWidth, newHeight);

        for (unsigned int y = 0; y < newHeight; ++y)
        {
            for (unsigned int c = 0; c < NUM_CHANNELS; ++c)
            {
                PixelType value = (GetPixelChannelValue(0, y/2, c) +
                    GetPixelChannelValue(0, y / 2 + 1, c)) * (PixelType)0.5;
                resized->SetPixelChannelValue(0, y, c, value);
            }
        }

        return resized;
    }
    else if (m_height == 1)
    {
        // Minimum height... only resize width
        unsigned int newWidth = m_width / 2;
        unsigned int newHeight = 1;
        PixelImage* resized = new PixelImage(newWidth, newHeight);

        for (unsigned int x = 0; x < newWidth; ++x)
        {
            for (unsigned int c = 0; c < NUM_CHANNELS; ++c)
            {
                PixelType value = (GetPixelChannelValue(x / 2, 0, c) +
                    GetPixelChannelValue(x / 2 + 1, 0, c)) * (PixelType)0.5;
                resized->SetPixelChannelValue(x, 0, c, value);
            }
        }

        return resized;
    }
    else
    {
        // Resize both.
        unsigned int newWidth = m_width / 2;
        unsigned int newHeight = m_height / 2;
        PixelImage* resized = new PixelImage(newWidth, newHeight);
        
        for (unsigned int y = 0; y < newHeight; ++y)
        {
            for (unsigned int x = 0; x < newWidth; ++x)
            {
                for (unsigned int c = 0; c < NUM_CHANNELS; ++c)
                {
                    PixelType value =  (GetPixelChannelValue(x * 2, y * 2, c) +
                                        GetPixelChannelValue(x * 2 + 1, y * 2, c) +
                                        GetPixelChannelValue(x * 2, y * 2 + 1, c) +
                                        GetPixelChannelValue(x * 2 + 1, y * 2 + 1, c)) * (PixelType)0.25;
                    resized->SetPixelChannelValue(x, y, c, value);
                }
            }
        }

        return resized;
    }
}

#ifdef _DEBUG
void PixelImage::DebugPrintImage8Bit() const
{
    for (unsigned int y = 0; y < m_height; ++y)
    {
        for (unsigned int x = 0; x < m_width; ++x)
        {
            int r = (int)((GetPixelChannelValue(x, y, 0) * 255.0) + 0.5);
            int g = (int)((GetPixelChannelValue(x, y, 1) * 255.0) + 0.5);
            int b = (int)((GetPixelChannelValue(x, y, 2) * 255.0) + 0.5);
            printf("(%u, %u) = (%u, %u, %u)\n", x, y, r, g, b);
        }
    }
}
#endif

