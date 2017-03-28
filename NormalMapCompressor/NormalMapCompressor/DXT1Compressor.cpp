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

Vector3 DecompressColor(const int& color)
{
    float red = (color >> 11) / 31.0;
    float green = ((color & 2016) >> 5) / 63.0;
    float blue = (color & 31) / 31.0;
    return Vector3(red, green, blue);
}

// calculate sum of squared distance to closest of the 4 points.
float CalculateColorScore(const float* lineVals, const int pxCount, const float& pt0, const float& pt3)
{
    float sum = 0.0;
    float pt1 = (pt0 * 2.0 + pt3) * 0.3333333;
    float pt2 = (pt3 * 2.0 + pt0) * 0.3333333;
    for (int i = 0; i < pxCount; ++i)
    {
        float diff0 = lineVals[i] - pt0;
        float diff1 = lineVals[i] - pt1;
        float diff2 = lineVals[i] - pt2;
        float diff3 = lineVals[i] - pt3;
        diff0 *= diff0;
        diff1 *= diff1;
        diff2 *= diff2;
        diff3 *= diff3;
        sum += fmin(fmin(fmin(diff0, diff1), diff2), diff3);
    }

    return sum;
}

void GetMaximumAndMinimum(const float* lineVals, const int pxCount, float& min, float& max)
{
    max = lineVals[0];
    min = lineVals[0];
    for (int i = 1; i < pxCount; ++i)
    {
        max = fmax(max, lineVals[i]);
        min = fmin(min, lineVals[i]);
    }
}

// Brute force of finding (approximately) the best two points that -- with two additional
// points interpolated evenly between them -- will minimize the sum of squared distance
// between the given point list and the closest of the 4 points.
void FindOptimalColorPoints(const float* lineVals, const int pxCount, float& pt0, float& pt3)
{
    float min, max;
    GetMaximumAndMinimum(lineVals, pxCount, min, max);
    float diff = max - min;
    float bestConfig[2] = { min, max };
    float bestScore = CalculateColorScore(lineVals, pxCount, pt0, pt3);
    const int numSteps = 64;
    for (int p0 = 0; p0 < numSteps - 1; ++p0)
    {
        pt0 = (p0 / numSteps) * diff + min;
        for (int p1 = p0 + 1; p1 < numSteps; ++p1)
        {
            pt3 = (p1 / numSteps) * diff + min;
            float score = CalculateColorScore(lineVals, pxCount, pt0, pt3);
            if (score < bestScore)
            {
                bestScore = score;
                bestConfig[0] = pt0;
                bestConfig[1] = pt3;
            }
        }
    }
    
    pt0 = bestConfig[0];
    pt3 = bestConfig[1];
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
    
    // pick color_0 and color_1 based on trying to minimize the sum of squared distances
    // between a point on the line, and the nearest of the 4 points (point 1 and 2, and 2
    // points evenly spaced between them).

    float* lineVals = (float*)malloc(sizeof(float) * pxCount);
    for (int i = 0; i < pxCount; ++i)
    {
        lineVals[i] = offsets[i].dot(bestFitVector);
    }

    float minValue;
    float maxValue;
    FindOptimalColorPoints(lineVals, pxCount, minValue, maxValue);
    
    Vector3 minColor = centroid + bestFitVector * minValue;
    Vector3 maxColor = centroid + bestFitVector * maxValue;

    // DXT-1 uses the ordering of the colors to determine if alpha is present or not.
    // Since we never want alpha channel for normal map, we must always ensure that
    // the integer representation of color1 is greater than color0.

    // switching to the... rather deceptive names that the dxt format gives the colors.
    // color 0 and color 1 are the colors that color2 and color 3 interpolate between. :/
    int minColorComp = CompressColor(minColor);
    int maxColorComp = CompressColor(maxColor);

    if (minColorComp < maxColorComp)
    {
        block.color1 = minColorComp;
        block.color0 = maxColorComp;
    }
    else if (maxColorComp < minColorComp)
    {
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

    // Calculate the "interp" value of each pixel in the block.  This value is one of
    // four possibilities to tell the decompressor which of the 4 colors of the block
    // to use as the color:
    // 0 - use color_0
    // 1 - use color_1
    // 2 - interpolate: 2 parts color_0, 1 part color_1
    // 3 - interpolate: 1 part color_0, 2 parts color_1

    // Initialize values to color1.  A block might not hit all pixels, so make sure they're
    // set to 1.
    for (int i = 0; i < 16; ++i)
    {
        block.interp[i] = 1;
    }

    Vector3 colors[4];
    colors[0] = DecompressColor(block.color0);
    colors[1] = DecompressColor(block.color1);
    colors[2] = (colors[0] * 2.0 + colors[1]) * 0.33333333;
    colors[3] = (colors[1] * 2.0 + colors[0]) * 0.33333333;

    // Set color interp value to the closest of the four colors.
    for (int i = 0; i < pxCount; ++i)
    {
        int x = i % blockWidth;
        int y = i / blockWidth;
        int index = y * 4 + x;

        Vector3& sourceColor = sourceColors[index];

        int bestIndex = 0;
        float bestValue = (sourceColor - colors[0]).GetLengthSquared();
        for (int j = 1; j < 4; ++j)
        {
            float distSq = (sourceColor - colors[j]).GetLengthSquared();
            if (distSq < bestValue)
            {
                bestIndex = j;
                bestValue = distSq;
            }
        }

        block.interp[index] = bestIndex;
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
            printf("calculating block %d, %d...\n", bx, by);
            ConstructPixelBlockFromImage(image, cImg->blocks[blockIndex], bx, by);
        }
    }

    return cImg;
}

