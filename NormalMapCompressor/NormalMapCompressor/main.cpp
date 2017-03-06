
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
    
    //PixelImage* psdInput = ReadPSDFile("C:\\Users\\trevo\\Desktop\\psdTests\\ideal_alpha.psd");
    //PixelImage* psdInput = ReadPSDFile(argv[1]);

    /*
    // Test image resizing.
    PixelImage* img = new PixelImage(8, 8);
    for (unsigned int y = 0; y < 8; ++y)
    {
        for (unsigned int x = 0; x < 8; ++x)
        {
            unsigned int flip = (x + y) % 2;
            img->SetPixelChannelValue(x, y, 0, (double)flip);
            img->SetPixelChannelValue(x, y, 1, (double)flip);
            img->SetPixelChannelValue(x, y, 2, (double)flip);
        }
    }

    img->DebugPrintImage8Bit();

    std::cout << "\nNewImage:\n";

    PixelImage* newImg = img->GetResized(4, 4);
    newImg->DebugPrintImage8Bit();
    */

    
    // Test fast mip resizing.
    PixelImage* img = new PixelImage(8, 32);
    for (unsigned int y = 0; y < img->GetHeight(); ++y)
    {
        for (unsigned int x = 0; x < img->GetWidth(); ++x)
        {
            unsigned int flip = (x + y) % 2;
            img->SetPixelChannelValue(x, y, 0, (PixelType)flip);
            img->SetPixelChannelValue(x, y, 1, (PixelType)flip);
            img->SetPixelChannelValue(x, y, 2, (PixelType)flip);
        }
    }

    img->DebugPrintImage8Bit();

    std::cout << "\nMipping...:";

    PixelImage* newImg = img->FastMipResize();
    int mipLevel = 1;
    while (newImg != NULL)
    {
        std::cout << "\nMip level = " << mipLevel++ << std::endl;
        newImg->DebugPrintImage8Bit();
        newImg = newImg->FastMipResize();
    }

}