#include "PixelImage.h"

#pragma once

struct PixelBlock
{
    PixelType red0;
    PixelType green0;
    PixelType blue0;

    PixelType red1;
    PixelType green1;
    PixelType blue1;

    float interp[16];
};

PixelBlock* ConvertImageToBlocks(PixelImage* image);
