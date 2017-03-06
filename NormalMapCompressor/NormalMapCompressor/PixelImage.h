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

    /**
     * Returns a NEW image, resized.  Old one remains untouched.
     */
    PixelImage* GetResized(unsigned int newWidth, unsigned int newHeight) const;

    /**
     * Returns a half-rez copy of the image.  Image dimensions prior to resize MUST be
     * an even number (better yet, power of two, as this is called many many times).
     */
    PixelImage* FastMipResize() const;


#ifdef _DEBUG
    void DebugPrintImage8Bit() const;
#endif


    unsigned int GetWidth()  { return m_width;  }
    unsigned int GetHeight() { return m_height; }

private:
    const unsigned int          m_width;
    const unsigned int          m_height;
    PixelType**                 m_channels;
};

#endif // PIXEL_IMAGE_H

