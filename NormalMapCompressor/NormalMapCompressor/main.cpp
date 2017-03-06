
#include <cstdio>
#include <iostream>
#include "PSDReader.h"
#include "PixelImage.h"

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
    
    PixelImage* psdInput = ReadPSDFile("C:\\Users\\trevo\\Desktop\\psdTests\\ideal_alpha.psd");
    //PixelImage* psdInput = ReadPSDFile(argv[1]);
    unsigned int width = psdInput->GetWidth();
    unsigned int height = psdInput->GetHeight();
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            int r = (int)((psdInput->GetPixelChannelValue(x, y, 0) * 255.0) + 0.5);
            int g = (int)((psdInput->GetPixelChannelValue(x, y, 1) * 255.0) + 0.5);
            int b = (int)((psdInput->GetPixelChannelValue(x, y, 2) * 255.0) + 0.5);
            printf("(%u, %u) = (%u, %u, %u)\n", x, y, r, g, b);
        }
    }
    

}