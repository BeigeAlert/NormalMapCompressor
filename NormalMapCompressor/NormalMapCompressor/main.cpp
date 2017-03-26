
#include <cstdio>
#include <iostream>
#include "PSDReader.h"
#include "PixelImage.h"
#include "Vector3.h"
#include "Matrix3x3.h"

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

    /*
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
    */

    // TEST DECOMPOSITION

    //Vector3* vects = (Vector3*)malloc(sizeof(Vector3) * 8);
    /*
    vects[0] = Vector3(-1, -0.5, -0.5);
    vects[1] = Vector3(-1,  0.5, -0.5);
    vects[2] = Vector3(-1, -0.5,  0.5);
    vects[3] = Vector3(-1,  0.5,  0.5);
    vects[4] = Vector3( 1, -0.5, -0.5);
    vects[5] = Vector3( 1,  0.5, -0.5);
    vects[6] = Vector3( 1, -0.5,  0.5);
    vects[7] = Vector3( 1,  0.5,  0.5);
    */

    /*
    Vector3 X = Vector3(0.8666, 0.5, 0.0);
    Vector3 Y = Vector3(-0.5, 0.8666, 0.0);
    Vector3 Z = Vector3(0.0, 0.0, 1.0);
    */
    /*
    Vector3 X = Vector3(0.5, 0.8666, 0.0);
    Vector3 Y = Vector3(-0.8666, 0.5, 0.0);
    Vector3 Z = Vector3(0.0, 0.0, 1.0);

    vects[0] = X * 8 + Y * 2 + Z * 2;
    vects[1] = X * 8 - Y * 2 + Z * 2;
    vects[2] = X * 8 + Y * 2 - Z * 2;
    vects[3] = X * 8 - Y * 2 - Z * 2;
    vects[4] = X * -8 + Y * 2 + Z * 2;
    vects[5] = X * -8 - Y * 2 + Z * 2;
    vects[6] = X * -8 + Y * 2 - Z * 2;
    vects[7] = X * -8 - Y * 2 - Z * 2;
    */
    /*
    vects[0] = Vector3(-1.0, -1.0, -1.0);
    vects[1] = Vector3(-1.0, -1.0,  1.0);
    vects[2] = Vector3(-1.0,  1.0, -1.0);
    vects[3] = Vector3(-1.0,  1.0,  1.0);
    vects[4] = Vector3( 1.0, -1.0, -1.0);
    vects[5] = Vector3( 1.0, -1.0,  1.0);
    vects[6] = Vector3( 1.0,  1.0, -1.0);
    vects[7] = Vector3( 1.0,  1.0,  1.0);

    for (int i = 0; i < 8; ++i)
    {
        vects[i].Print();
    }

    Matrix3x3 cov = Matrix3x3();
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            float sum = 0.0;
            for (int k = 0; k < 8; ++k)
            {
                sum += vects[k][r] * vects[k][c];
            }
            *cov(r, c) = sum/7;
        }
    }

    std::cout << "Covariance matrix:" << std::endl;
    cov.Print();
    std::cout << std::endl;

    Matrix3x3 rotation = Matrix3x3();
    Vector3 scale = cov.SpectralDecomposition(rotation);

    std::cout << "Rotation matrix:" << std::endl;
    rotation.Print();

    std::cout << std::endl;
    std::cout << "Scale:" << std::endl;
    scale.Print();

    std::cout << std::endl;
    */

}