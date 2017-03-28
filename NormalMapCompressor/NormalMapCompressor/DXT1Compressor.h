#include "PixelImage.h"
#include "Vector3.h"
#include <string>

#pragma once

struct PixelBlock
{
    int color0;
    int color1;
    int interp[16]; // 2 bits each
};

struct CompressedImage
{
    PixelBlock* blocks;
    int blockCountX;
    int blockCountY;
};

struct CompressedMipMapTexture
{
    CompressedImage** mips;
    int numMips;
    int sizeX;
    int sizeY;
};

CompressedImage* ConvertImageToBlocks(PixelImage* image);

int CompressColor(const Vector3& color);

std::string DebugColorBits(int color);

void ConstructPixelBlockFromImage(PixelImage* image, PixelBlock& block, int bx, int by);

