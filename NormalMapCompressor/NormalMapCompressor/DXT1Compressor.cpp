#include "DXT1Compressor.h"
#include <math.h>
#include <algorithm>
#include "Matrix3x3.h"
#include <iostream>
#include "Vector3.h"

std::string DebugColorBits(int color)
{
    int value = 1 << 15;
    std::string result = "00000 000000 00000";
    int index = 0;
    while (value > 0)
    {
        if (index == 5 || index == 12)
        {
            ++index;
        }

        if (color & value)
        {
            result[index] = '1';
        }

        value = value >> 1;
        ++index;
    }

    return result;
}

int CompressColor(const Vector3& color)
{
    // DXT-1 (no alpha) stores RGB as 16 bit values. rrrrr gggggg bbbbb
    int value = 0;
    value += (int)(color.r * ((1 << 5)-1) + 0.5) << 11;
    value += (int)(color.g * ((1 << 6)-1) + 0.5) << 5;
    value += (int)(color.b * ((1 << 5)-1) + 0.5);

    return value;
}

void ConstructPixelBlockFromImage(PixelImage* image, PixelBlock& block, int bx, int by)
{

    unsigned int topLeftX = bx * 4;
    unsigned int topLeftY = by * 4;
    unsigned int botRightX = std::min(topLeftX + 3, image->GetWidth() - 1);
    unsigned int botRightY = std::min(topLeftY + 3, image->GetHeight() - 1);

    int pxCount = ((botRightX - topLeftX) + 1) * ((botRightY - topLeftY) + 1);
    int blockWidth = (botRightX - topLeftX) + 1;
    int blockHeight = (botRightY - topLeftY) + 1;

    Vector3* sourceColors = (Vector3*)malloc(sizeof(Vector3) * pxCount);
    for (int y = 0; y < blockHeight; ++y)
    {
        for (int x = 0; x < blockWidth; ++x)
        {
            int index = y * blockWidth + x;
            sourceColors[index] = Vector3();
            sourceColors[index].r = image->GetPixelChannelValue(x + topLeftX, y + topLeftY, 0);
            sourceColors[index].g = image->GetPixelChannelValue(x + topLeftX, y + topLeftY, 1);
            sourceColors[index].b = image->GetPixelChannelValue(x + topLeftX, y + topLeftY, 2);
        }
    }

    // To determine the color_0 and color_1 values, we calculate the line of best fit
    // of each of the 16 source colors in 3d space.  Our two colors will lie on this
    // line.

    // Calculate centroid
    Vector3 centroid = Vector3();
    for (int i = 0; i < pxCount; ++i)
    {
        centroid += sourceColors[i];
    }
    centroid /= float(pxCount);

    // Calculate each color difference from centroid
    Vector3* offsets = (Vector3*)malloc(sizeof(Vector3) * pxCount);
    for (int i = 0; i < pxCount; ++i)
    {
        offsets[i] = sourceColors[i] - centroid;
    }

    // Create the covariance matrix
    Matrix3x3 cov = Matrix3x3();
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            float sum = 0.0;
            for (int k = 0; k < pxCount; ++k)
            {
                sum += offsets[k].data[r] * offsets[k].data[c];
            }
            *cov(r,c) = sum / (pxCount - 1);
        }
    }

    // Decompose it
    Matrix3x3 rotation = Matrix3x3();
    Vector3 scale = cov.SpectralDecomposition(rotation);

    // Pick vector with largest component
    int largest = 0;
    for (int i = 1; i < 3; ++i)
    {
        if (fabs(scale[i]) > fabs(scale[largest]))
        {
            largest = i;
        }
    }

    Vector3 bestFitVector = rotation.GetColumn(largest);
    
    // pick color_0 and color_1 based on max and min colors along the line...
    // would almost certainly be better to choose colors based on the 4 evenly
    // spaced colors that minimize sum of squared differences between the 16 pixel
    // colors... but I don't have a clue how to do that. :(
    // the method I'm settling for means that outliers will heavily influence the
    // result... but maybe it will be okay... :/

    float* lineVals = (float*)malloc(sizeof(float) * pxCount);
    for (int i = 0; i < pxCount; ++i)
    {
        lineVals[i] = offsets[i].dot(bestFitVector);
    }

    // find the min and max values on the line
    float minValue = lineVals[0];
    float maxValue = lineVals[0];
    int minValueIndex = 0;
    int maxValueIndex = 0;
    for (int i = 1; i < pxCount; ++i)
    {
        if (lineVals[i] < minValue)
        {
            minValue = lineVals[i];
            minValueIndex = i;
        }

        if (lineVals[i] > maxValue)
        {
            maxValue = lineVals[i];
            maxValueIndex = i;
        }
    }

    Vector3& minColor = sourceColors[minValueIndex];
    Vector3& maxColor = sourceColors[maxValueIndex];

    // DXT-1 uses the ordering of the colors to determine if alpha is present or not.
    // Since we never want alpha channel for normal map, we must always ensure that
    // the integer representation of color1 is greater than color0.

    // switching to the... rather deceptive names that the dxt format gives the colors.
    // color 0 and color 1 are the colors that color2 and color 3 interpolate between. :/
    int color0Index;
    int color1Index;
    int minColorComp = CompressColor(minColor);
    int maxColorComp = CompressColor(maxColor);

    if (minColorComp < maxColorComp)
    {
        color1Index = minValueIndex;
        color0Index = maxValueIndex;
        block.color1 = minColorComp;
        block.color0 = maxColorComp;
    }
    else if (maxColorComp < minColorComp)
    {
        color1Index = maxValueIndex;
        color0Index = minValueIndex;
        block.color1 = maxColorComp;
        block.color0 = minColorComp;
    }
    else
    {
        // if the colors are equal, we ensure we output all the same color.
        block.color0 = 0;
        block.color1 = minColorComp;
        for (int i = 0; i < 16; ++i)
        {
            block.interp[i] = 1; // only use color 1
        }

        return;
    }

    // Calculate the "interp" value of each pixel in the block.  This value is the
    // fraction of the value of color1 versus the value of color0 that will be the
    // final color.
    float color0LineVal = lineVals[color0Index];
    float color1LineVal = lineVals[color1Index];
    float diffInv = 1.0 / (color1LineVal - color0LineVal);

    // Initialize values to color1.  A block might not hit all pixels, so make sure they're
    // set to 1.
    for (int i = 0; i < 16; ++i)
    {
        block.interp[i] = 1;
    }

    for (int i = 0; i < pxCount; ++i)
    {
        float t = fmin(fmax((lineVals[i] - color0LineVal) * diffInv, 0.0), 1.0);
        int zone = (t * 3.0) + 0.5;

        int x = i % blockWidth;
        int y = i / blockWidth;

        // reverse the column order because... I don't know... that's the way it is in
        // the spec!
        // d c b a
        // h g f e
        // l k j i
        // p o n m
        //x = 3 - x;

        int index = y * 4 + x;

        int& interp = block.interp[index];

        switch(zone)
        {
        case 0:
            interp = 0;
            break;
        case 1:
            interp = 2;
            break;
        case 2:
            interp = 3;
            break;
        case 3:
            interp = 1;
            break;
        default:
            std::cerr << "something went wrong!!!" << std::endl;
        }
    }
}

CompressedImage* ConvertImageToBlocks(PixelImage* image)
{
    CompressedImage* cImg = new CompressedImage();
    cImg->blockCountX = ceil(image->GetWidth() / 4.0);
    cImg->blockCountY = ceil(image->GetHeight() / 4.0);

    cImg->blocks = (PixelBlock*)malloc(sizeof(PixelBlock) * cImg->blockCountX * cImg->blockCountY);

    for (int by = 0; by < cImg->blockCountY; ++by)
    {
        for (int bx = 0; bx < cImg->blockCountX; ++bx)
        {
            int blockIndex = by * cImg->blockCountX + bx;
            ConstructPixelBlockFromImage(image, cImg->blocks[blockIndex], bx, by);
        }
    }

    return cImg;
}

