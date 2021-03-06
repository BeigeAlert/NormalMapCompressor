
#include <cstdio>
#include <iostream>
#include "PSDReader.h"
#include "PixelImage.h"
#include "Vector3.h"
#include "Matrix3x3.h"
#include "DXT1Compressor.h"
#include <math.h>
#include <algorithm>
#include "DDSWriter.h"
#include <fstream>

int main(int argc, char* argv[])
{
    /*
    if (argc != 3)
    {
        std::cout <<
            "Natural Selection 2 Normal Map Compressor" << std::endl <<
            "(PSD -> DDS converter with very limited capabilities)" << std::endl <<
            std::endl <<
            "   Written by Trevor Harris (trevor@naturalselection2.com)" << std::endl <<
            std::endl <<
            "Usage:" << std::endl << std::endl <<
            "    SparkNormalCompressor \"input\\file\\path\\name.psd\" \"output\\file\\path\\name.dds\"" << std::endl << std::endl <<
            "Limitations:" << std::endl <<
            "    - RGB mode only" << std::endl <<
            "    - 8 bits per channel only" << std::endl <<
            "    - Alpha channels and transparency are ignored" << std::endl << std::endl;
        exit(0);
    }
    */
    
    PixelImage* psdInput = ReadPSDFile("C:\\Users\\trevo\\Desktop\\psdTests\\veil_wall_01_b_normal.psd");
    //PixelImage* psdInput = ReadPSDFile(argv[1]);

    // Ensure image dimensions are powers of 2.  If not, increase them to the nearest
    // power of 2.
    int newWidth = pow(2, ceil(log2(psdInput->GetWidth())));
    int newHeight = pow(2, ceil(log2(psdInput->GetHeight())));

    PixelImage* resizedInput = psdInput;
    if (newWidth != psdInput->GetWidth() || newHeight != psdInput->GetHeight())
    {
        resizedInput = psdInput->GetResized(newWidth, newHeight);
    }

    // Create mip maps for image.
    int numLevels = (int)log2(std::max(newWidth, newHeight));
    PixelImage** mips = (PixelImage**)malloc(sizeof(PixelImage*) * numLevels);

    mips[0] = resizedInput;
    for (int i = 1; i < numLevels; ++i)
    {
        mips[i] = mips[i - 1]->FastMipResize();
    }

    // Compress the data.
    CompressedMipMapTexture dds;
    dds.numMips = numLevels;
    dds.sizeX = newWidth;
    dds.sizeY = newHeight;
    dds.mips = (CompressedImage**)malloc(sizeof(CompressedImage*) * numLevels);
    for (int i = 0; i < numLevels; ++i)
    {
        dds.mips[i] = ConvertImageToBlocks(mips[i]);
    }

    //WriteDDSFile(&dds, argv[2]);
    WriteDDSFile(&dds, "C:\\users\\trevo\\desktop\\test.dds");
    
    /*
    PixelImage* debugImage = new PixelImage(4, 4);
    for (int i = 0; i < 4; ++i)
    {
        for (int c = 0; c < 3; ++c)
        {
            for (int y = 0; y < 4; ++y)
            {
                debugImage->SetPixelChannelValue(i, y, c, i * 0.33333);
            }
        }
    }

    PixelBlock debugPixelBlock;
    ConstructPixelBlockFromImage(debugImage, debugPixelBlock, 0, 0);

    printf("color0 = %d\n", debugPixelBlock.color0);
    printf("color1 = %d\n", debugPixelBlock.color1);
    printf("interps:\n");
    for (int y = 0; y < 4; ++y)
    {
        printf("|");
        for (int x = 0; x < 4; ++x)
        {
            printf(" %d ", debugPixelBlock.interp[y * 4 + x]);
        }
        printf("|\n");
    }
    */
    
#ifdef _DEBUG
    // add a pause at the end so we can see console output... only in debug though.
    std::cout << "done." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
#endif

}