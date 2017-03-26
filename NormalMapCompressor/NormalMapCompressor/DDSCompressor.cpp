#include "DDSCompressor.h"
#include <math.h>
#include <algorithm>
#include "Matrix3x3.h"
#include "Vector3.h"

void ConstructPixelBlockFromImage(PixelImage* image, PixelBlock& block, int bx, int by)
{
    // TODO return early if only one pixel. (eg lowest mip level)

    unsigned int topLeftX = bx * 4;
    unsigned int topLeftY = by * 4;
    unsigned int botRightX = std::min(topLeftX + 4, image->GetWidth() - 1);
    unsigned int botRightY = std::min(topLeftY + 4, image->GetHeight() - 1);

    int pxCount = ((botRightX - topLeftX) + 1) * ((botRightY - topLeftY) + 1);

    // To determine the color_0 and color_1 values, we calculate the line of best fit
    // of each of the 16 source colors in 3d space.  Our two colors will lie on this
    // line.

    // Calculate centroid
    Vector3 centroid = Vector3();
    for (int y = topLeftY; y <= botRightY; ++y)
    {
        for (int x = topLeftX; x <= botRightX; ++x)
        {
            centroid.x += image->GetPixelChannelValue(x, y, 0);
            centroid.y += image->GetPixelChannelValue(x, y, 1);
            centroid.z += image->GetPixelChannelValue(x, y, 2);
        }
    }
    centroid /= (float)pxCount;

    // Calculate each color difference from centroid
    Vector3* offsets = (Vector3*)malloc(sizeof(Vector3) * pxCount);
    int blockWidth = (botRightX - topLeftX) + 1;
    for (int y = topLeftY; y <= botRightY; ++y)
    {
        for (int x = topLeftX; x <= botRightX; ++x)
        {
            int index = y * blockWidth + x;
            offsets[index] = Vector3();
            Vector3& adjusted = offsets[index];
            adjusted.x = image->GetPixelChannelValue(x, y, 0);
            adjusted.y = image->GetPixelChannelValue(x, y, 1);
            adjusted.z = image->GetPixelChannelValue(x, y, 2);

            adjusted -= centroid;
        }
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
                sum += offsets[k][r] * offsets[k][c];
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

    Vector3 bestFitVector = rotation.GetRow(largest);
    
    // TODO pick color_0 and color_1

}

PixelBlock* ConvertImageToBlocks(PixelImage* image)
{
    int blockCountX = ceil(image->GetWidth() / 4.0);
    int blockCountY = ceil(image->GetHeight() / 4.0);

    PixelBlock* blocks = (PixelBlock*)malloc(sizeof(PixelBlock) * blockCountX * blockCountY);

    for (int by = 0; by < blockCountY; ++by)
    {
        for (int bx = 0; bx < blockCountX; ++bx)
        {
            int blockIndex = by * blockCountX + bx;
            PixelBlock block = blocks[blockIndex];

            ConstructPixelBlockFromImage(image, block, bx, by);

        }
    }

    return NULL;
}

