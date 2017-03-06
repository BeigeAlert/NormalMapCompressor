
#ifndef PSD_READER_H
#define PSD_READER_H

#include "PixelImage.h"

void CheckLength(const unsigned int& position, const unsigned int needed, const unsigned int length);

void SkipBytes(unsigned int& position, unsigned int skipCount, const unsigned int length);

unsigned int GetUInt4(const char* data, unsigned int& position, const unsigned int length);

unsigned int GetUInt2(const char* data, unsigned int& position, const unsigned int length);

unsigned int GetUInt1(const char* data, unsigned int& position, const unsigned int length);

unsigned int GetSInt1(const char* data, unsigned int& position, const unsigned int length);

char* ReadBinaryFile(const char* fileName, unsigned int& length);

struct ImageDetails
{
    unsigned int             numChannels;
    unsigned int    imageWidth;
    unsigned int    imageHeight;
};

PixelImage* ReadPSDDataRaw(const char* data, unsigned int& position, const unsigned int& length, ImageDetails imgDetails);

PixelImage* ReadPSDDataRLE(const char* data, unsigned int& position, const unsigned int& length, ImageDetails imgDetails);

PixelImage* ReadPSDFile(const char* fileName);

#endif // PSD_READER_H
