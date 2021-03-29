struct BMPHeader
{
    uint16 magic;
    uint32 fileSize;
    uint16 reserved1, reserved2;
    uint32 dataOffset, headerSize;
    int32 width, height;
    uint16 planes, bitsPerPixel;
    uint32 compression, imageDataSize;
    int32 hPixelsPerMeter, vPixelsPerMeter;
    uint32 coloursUsed, coloursRequired;
};

String BMPImageFormat::getFormatName()
{
    return "BMP";
}

bool BMPImageFormat::canUnderstand (InputStream& input)
{
    return input.readByte() == 'B' && input.readByte() == 'M';
}

bool BMPImageFormat::usesFileExtension (const File& possibleFile)
{
    return possibleFile.hasFileExtension ("bmp")
        || possibleFile.hasFileExtension ("dib");
}

Image BMPImageFormat::decodeImage (InputStream& input)
{
    BMPHeader header;
    header.magic           = (uint16) input.readShort();
    header.fileSize        = (uint32) input.readInt();
    header.reserved1       = (uint16) input.readShort();
    header.reserved2       = (uint16) input.readShort();
    header.dataOffset      = (uint32) input.readInt();
    header.headerSize      = (uint32) input.readInt();
    header.width           = (int32) input.readInt();
    header.height          = (int32) input.readInt();
    header.planes          = (uint16) input.readShort();
    header.bitsPerPixel    = (uint16) input.readShort();
    header.compression     = (uint32) input.readInt();
    header.imageDataSize   = (uint32) input.readInt();
    header.hPixelsPerMeter = (int32) input.readInt();
    header.vPixelsPerMeter = (int32) input.readInt();
    header.coloursUsed     = (uint32) input.readInt();
    header.coloursRequired = (uint32) input.readInt();

    if (header.compression != 0 || (header.bitsPerPixel != 8 && header.bitsPerPixel != 24 && header.bitsPerPixel != 32))
    {
        jassertfalse; // Unsupported BMP format
        return {};
    }

    if (header.bitsPerPixel == 8 && header.coloursUsed == 0)
        header.coloursUsed = 256;

    Array<PixelARGB> colourTable;

    for (int i = 0; i < int (header.coloursUsed); i++)
    {
        const auto b = (uint8) input.readByte();
        const auto g = (uint8) input.readByte();
        const auto r = (uint8) input.readByte();
        const auto a = (uint8) input.readByte();

        colourTable.add (PixelARGB (a, r, g, b));
    }

    const bool isBottomUp = header.height < 0;
    header.height = std::abs (header.height);

    Image image (Image::ARGB, (int) header.width, (int) header.height, true);
    Image::BitmapData data (image, Image::BitmapData::writeOnly);

    input.setPosition (header.dataOffset);

    const auto bytesPerPixel = header.bitsPerPixel / 8;
    const auto bytesPerRow = (int) (std::floor ((header.bitsPerPixel * header.width + 31) / 32.0) * 4);

    auto* rowData = new uint8[size_t (bytesPerRow)];
    for (int y = 0; y < int (header.height); y++)
    {
        input.read (rowData, bytesPerRow);

        for (int x = 0; x < int (header.width); x++)
        {
            auto* d = &rowData[x * bytesPerPixel];

            auto* p = (PixelARGB*) data.getPixelPointer (x, int (isBottomUp ? y : header.height - y - 1));
            if (header.bitsPerPixel == 8)
                *p = colourTable[d[0]];
            else
                p->setARGB (bytesPerPixel == 4 ? d[3] : 255, d[2], d[1], d[0]);
        }
    }

    delete[] rowData;

    return image;
}

bool BMPImageFormat::writeImageToStream (const Image& sourceImage, OutputStream& stream)
{
    auto image = sourceImage.convertedToFormat (Image::ARGB);

    stream.writeByte ('B');
    stream.writeByte ('M');
    stream.writeInt (40 + image.getWidth() * image.getHeight() * 4);
    stream.writeShort (0);
    stream.writeShort (0);
    stream.writeInt (54);
    stream.writeInt (40);
    stream.writeInt (image.getWidth());
    stream.writeInt (image.getHeight());
    stream.writeShort (1);
    stream.writeShort (32);
    stream.writeInt (0);
    stream.writeInt (image.getWidth() * image.getHeight() * 4);
    stream.writeInt (2835);
    stream.writeInt (2835);
    stream.writeInt (0);
    stream.writeInt (0);

    Image::BitmapData data (image, Image::BitmapData::readOnly);
    for (int y = 0; y < image.getHeight(); y++)
    {
        for (int x = 0; x < image.getWidth(); x++)
        {
            auto* p = (PixelARGB*) data.getPixelPointer (x, (int) image.getHeight() - y - 1);
            stream.writeByte ((int8) p->getBlue());
            stream.writeByte ((int8) p->getGreen());
            stream.writeByte ((int8) p->getRed());
            stream.writeByte ((int8) p->getAlpha());
        }
    }

    return true;
}
