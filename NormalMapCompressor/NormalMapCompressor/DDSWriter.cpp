#include <iostream>
#include <fstream>
#include <algorithm>

#include "DXT1Compressor.h"

#include "DDSWriter.h"

void WriteString(std::ofstream& writer, std::string str)
{
    writer.write(str.c_str(), sizeof(str.c_str()));
}

void WriteUInt4(std::ofstream& writer, unsigned int value)
{
    writer.write((char*)value, 4);
}

void WriteColorBlock(std::ofstream& writer, const PixelBlock& block)
{
    writer.write((char*)block.color0, 2);
    writer.write((char*)block.color1, 2);
    
    // each row is one byte (therefore each color is 2 bits)
    for (int row = 0; row < 4; ++row)
    {
        int colorByte = 0;
        for (int column = 0; column < 4; ++column)
        {
            int index = row * 4 + column;
            colorByte |= block.interp[index] << (column * 2);
        }

        writer.write((char*)colorByte, 1);
    }
}

void WriteDDSFile(const CompressedMipMapTexture* dds, const char* filePath)
{
    std::ofstream writer;
    writer.open(filePath, std::ios::out | std::ios::binary);

    WriteString(writer, "DDS\x20");

    WriteUInt4(writer, 124);

    unsigned int flags = 0;
    flags |= 0x1; // "required" flag
    flags |= 0x2; // "height" flag
    flags |= 0x4; // "width" flag
    flags |= 0x1000; // "pixelformat" flag
    flags |= 0x20000; // "mipmapcount" flag
    WriteUInt4(writer, flags);

    WriteUInt4(writer, dds->sizeY); // height
    int width = dds->sizeX;
    WriteUInt4(writer, dds->sizeX); // width

    // Pitch computed via this recommended formula from microsoft
    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb943991(v=vs.85).aspx
    // max( 1, ((width + 3)/4)) * block-size
    // (block size is 8 bytes for DXT1)
    unsigned int pitch = std::max(1, ((width + 3) / 4)) * 8.0;
    WriteUInt4(writer, pitch);

    // Depth
    WriteUInt4(writer, 0);

    // Mip map count (including top level)
    WriteUInt4(writer, dds->numMips);

    // 11 reserved DWORD values.
    for (int i = 0; i < 11; ++i)
    {
        WriteUInt4(writer, 0);
    }

    // Pixel format structure size
    WriteUInt4(writer, 32);

    // Flags (always the same, 0x4 = compressed)
    WriteUInt4(writer, 4);

    // FourCC of compression technique
    WriteString(writer, "DXT1");

    // 5 unused DWORD values
    for (int i = 0; i < 5; ++i)
    {
        WriteUInt4(writer, 0);
    }

    // Caps1 (flags)
    int caps1 = 0;
    caps1 |= 0x8; // "complex" flag (eg contains mip maps).
    caps1 |= 0x400000; // "mip map" flag
    caps1 |= 0x1000; // "texture" flag, required
    WriteUInt4(writer, caps1);

    // Caps2-5 unused for our purposes here.
    WriteUInt4(writer, 0);
    WriteUInt4(writer, 0);
    WriteUInt4(writer, 0);
    WriteUInt4(writer, 0);

    // Write the color block data.  Each mip map is written one after the other.
    for (int i = 0; i < dds->numMips; ++i)
    {
        CompressedImage* mip = dds->mips[i];
        for (int y = 0; y < mip->blockCountY; ++y)
        {
            for (int x = 0; x < mip->blockCountX; ++x)
            {
                int index = y * mip->blockCountX + x;
                WriteColorBlock(writer, mip->blocks[index]);
            }
        }
        
    }

    writer.close();
}

