#ifndef PIXEL_IMAGE_H
#define PIXEL_IMAGE_H

#include <stdlib.h>

typedef float PixelType;

#define NUM_CHANNELS 3 // only care about RGB right now.

class PixelImage
{
public:
    
    PixelImage(unsigned int width, unsigned int height);
    ~PixelImage();

    void SetPixelChannelValue(unsigned int x, unsigned int y, unsigned int channelNumber, PixelType value);

    PixelType GetPixelChannelValue(unsigned int x, unsigned int y, unsigned int channelNumber) const;

    unsigned int GetWidth()  { return m_width;  }
    unsigned int GetHeight() { return m_height; }

private:
    const unsigned int          m_width;
    const unsigned int          m_height;
    PixelType**                 m_channels;
};

#endif // PIXEL_IMAGE_H

