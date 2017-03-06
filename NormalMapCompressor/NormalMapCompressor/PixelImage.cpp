#include "PixelImage.h"

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

