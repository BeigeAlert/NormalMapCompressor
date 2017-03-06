
#include <iostream>
#include <fstream>

#include "PSDReader.h"
#include "PixelImage.h"

void CheckLength(const unsigned int& position, const unsigned int needed, const unsigned int length)
{
    if (position + needed > length)
    {
        std::cout << "Unexpected end of file while reading PSD." << std::endl;
        exit(1);
    }
}

void SkipBytes(unsigned int& position, unsigned int skipCount, const unsigned int length)
{
    CheckLength(position, skipCount, length);
    position += skipCount;
}

unsigned int GetUInt4(const char* data, unsigned int& position, const unsigned int length)
{
    CheckLength(position, 4, length);
    unsigned int result = (((unsigned char)data[position]) << 24) + (((unsigned char)data[position + 1]) << 16) + (((unsigned char)data[position + 2]) << 8) + ((unsigned char)data[position + 3]);
    position += 4;
    return result;
}

unsigned int GetUInt2(const char* data, unsigned int& position, const unsigned int length)
{
    CheckLength(position, 2, length);
    unsigned int result = (((unsigned char)data[position]) << 8) + ((unsigned char)data[position + 1]);
    position += 2;
    return result;
}

unsigned int GetUInt1(const char* data, unsigned int& position, const unsigned int length)
{
    CheckLength(position, 1, length);
    unsigned int result = ((unsigned char)data[position]);
    position += 1;
    return result;
}

unsigned int GetSInt1(const char* data, unsigned int& position, const unsigned int length)
{
    CheckLength(position, 1, length);
    int result = data[position];
    position += 1;
    return result;
}

char* ReadBinaryFile(const char* fileName, unsigned int& length)
{
    std::ifstream fileInput(fileName, std::ios::in | std::ios::binary | std::ios::ate);
    if (!fileInput.is_open())
    {
        std::cout << "Error opening file \"" << fileName << "\"." << std::endl;
        return NULL;
    }

    long size = fileInput.tellg();
    char* memblock = new char[size];
    fileInput.seekg(0, std::ios::beg);
    fileInput.read(memblock, size);
    fileInput.close();

    length = (unsigned int)size;
    return memblock;
}

PixelImage* ReadPSDDataRaw(const char* data, unsigned int& position, const unsigned int& length, ImageDetails imgDetails)
{
    PixelImage* result = new PixelImage(imgDetails.imageWidth, imgDetails.imageHeight);

    for (unsigned int c = 0; c < imgDetails.numChannels; ++c)
    {
        for (unsigned int y = 0; y < imgDetails.imageHeight; ++y)
        {
            for (unsigned int x = 0; x < imgDetails.imageHeight; ++x)
            {
                unsigned int value = GetUInt1(data, position, length);
                
                // ignore transparency/alpha channels, but still move over the bytes
                if (c < 3)
                {
                    result->SetPixelChannelValue(x, y, c, ((float)value) / 255.0f);
                }
            }
        }
    }

    return result;
}

PixelImage* ReadPSDDataRLE(const char* data, unsigned int& position, const unsigned int& length, ImageDetails imgDetails)
{
    PixelImage* result = new PixelImage(imgDetails.imageWidth, imgDetails.imageHeight);
    
    // skip over scanline byte counts... we live dangerously here... ;)
    for (unsigned int c = 0; c < imgDetails.numChannels; ++c)
    {
        for (unsigned int y = 0; y < imgDetails.imageHeight; ++y)
        {
            SkipBytes(position, 2, length);
        }
    }
    
    for (unsigned int c = 0; c < imgDetails.numChannels; ++c)
    {
        unsigned long long pxCount = imgDetails.imageWidth * imgDetails.imageHeight;
        unsigned long long pxPos = 0;
        while (pxPos < pxCount)
        {
            int header = GetSInt1(data, position, length);
            
            if (header >= 0)
            {
                // 0 - 127
                header++;
                for (int b = 0; b < header; ++b)
                {
                    unsigned int x = pxPos % imgDetails.imageWidth;
                    unsigned int y = pxPos / imgDetails.imageWidth;
                    unsigned int value = GetUInt1(data, position, length);
                    if (c < 3)
                    {
                        // Don't write non rgb channels
                        result->SetPixelChannelValue(x, y, c, ((float)value) / 255.0f);
                    }
                    pxPos++;
                }
            }
            else if (header > -127)
            {
                // -1 - -127
                header = 1 - header;
                unsigned int value = GetUInt1(data, position, length);
                for (int b = 0; b < header; ++b)
                {
                    unsigned int x = pxPos % imgDetails.imageWidth;
                    unsigned int y = pxPos / imgDetails.imageWidth;
                    if (c < 3)
                    {
                        // Don't write non rgb channels
                        result->SetPixelChannelValue(x, y, c, ((float)value) / 255.0f);
                    }
                    pxPos++;
                }
            }
            // -128, ignore.
        }
    }

    return result;
}

PixelImage* ReadPSDFile(const char* fileName)
{
    unsigned int length = 0;
    char* data = ReadBinaryFile(fileName, length);

    // Magic number must be 8BPS
    if (memcmp(data, "8BPS", 4) != 0)
    {
        std::cout << "Invalid file format.  Must be Adobe Photoshop (PSD) file." << std::endl;
        exit(1);
    }

    // Reserved 1 word.
    unsigned int position = 4;
    if (GetUInt2(data, position, length) != 1)
    {
        std::cout << "Invalid file format.  Must be Adobe Photoshop (PSD) file." << std::endl;
        exit(1);
    }

    // 3 Reserved 0 words.
    for (int i = 0; i < 3; ++i)
    {
        if (GetUInt2(data, position, length) != 0)
        {
            std::cout << "Invalid file format.  Must be Adobe Photoshop (PSD) file." << std::endl;
            exit(1);
        }
    }

    // Read num channels, width, and height from file.
    ImageDetails imgDetails = ImageDetails();
    imgDetails.numChannels = GetUInt2(data, position, length);
    imgDetails.imageHeight = GetUInt4(data, position, length);
    imgDetails.imageWidth = GetUInt4(data, position, length);

    // Ensure bit depth is 8... we only support 8 bits per channel right now.
    if (GetUInt2(data, position, length) != 8)
    {
        std::cout << "Unsupported bit depth detected!  Only 8-bits per channel is supported at the moment.  Sorry!" << std::endl;
        exit(1);
    }

    // Ensure color mode is RGB (3).
    if (GetUInt2(data, position, length) != 3)
    {
        std::cout << "Unsupported color mode detected!  Only RGB color is supported." << std::endl;
        exit(1);
    }

    // Ensure length of "color mode data" is zero... it must be if we're RGB, otherwise
    // it's an error.
    if (GetUInt4(data, position, length) != 0)
    {
        std::cout << "Error reading PSD file!  (Color mode data was non-zero)." << std::endl;
        exit(1);
    }

    // Skip "Image Resource" section.
    {
        int skipLength = GetUInt4(data, position, length);
        SkipBytes(position, skipLength, length);
    }

    // Skip "Layer and Mask Info" section.
    {
        int skipLength = GetUInt4(data, position, length);
        SkipBytes(position, skipLength, length);
    }

    PixelImage* result;
    int compressionMethod = GetUInt2(data, position, length);
    if (compressionMethod == 0)
    {
        // Load raw data
        result = ReadPSDDataRaw(data, position, length, imgDetails);
    }
    else if (compressionMethod == 1)
    {
        // Load RLE data
        result = ReadPSDDataRLE(data, position, length, imgDetails);
    }
    else
    {
        std::cout << "Unsupported compression scheme was used by Photoshop when saving document!  Scream at Beige to fix it!!! (scheme was " << compressionMethod << ")." << std::endl;
        exit(1);
    }

    // At this point, we should have read the entire file...
    if (position < length)
    {
        std::cout << "WARNING: File not completely read... something's not right!  Yell at Beige." << std::endl;
    }

    return result;

}

