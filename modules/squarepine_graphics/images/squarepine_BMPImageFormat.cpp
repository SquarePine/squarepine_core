struct BMPDIBHeader final
{
    BMPDIBHeader() noexcept = default;

    bool read (InputStream& input)
    {
        // BMP section:
        magicA          = (uint8) input.readByte();
        magicB          = (uint8) input.readByte();
        fileSize        = (uint32) input.readInt();
        reserved1       = (uint16) input.readShort();
        reserved2       = (uint16) input.readShort();
        dataOffset      = (uint32) input.readInt();

        // DIB section:
        headerSize      = (uint32) input.readInt();
        width           = (int32) input.readInt();
        height          = (int32) input.readInt();
        planes          = (uint16) input.readShort();
        bitsPerPixel    = (uint16) input.readShort();
        compression     = (uint32) input.readInt();
        imageDataSize   = (uint32) input.readInt();
        hPixelsPerMeter = (int32) input.readInt();
        vPixelsPerMeter = (int32) input.readInt();
        coloursUsed     = (uint32) input.readInt();
        coloursRequired = (uint32) input.readInt();

        switch (bitsPerPixel)
        {
            // Supported!
            case 8:
            case 24:
            case 32:
            break;

            // :( Unsupported!
            default:
                return false;
        };

        // Only support uncompressed:
        return compression == 0;
    }

    uint8 magicA = 0, magicB = 0;
    uint16 magic = 0;
    uint32 fileSize = 0;
    uint16 reserved1 = 0, reserved2 = 0;
    uint32 dataOffset = 0, headerSize = 0;
    int32 width = 0, height = 0;
    uint16 planes = 0, bitsPerPixel = 0;
    uint32 compression = 0, imageDataSize = 0;
    int32 hPixelsPerMeter = 0, vPixelsPerMeter = 0;
    uint32 coloursUsed = 0, coloursRequired = 0;
};

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
    BMPDIBHeader header;
    if (! header.read (input))
    {
        jassertfalse; // Unsupported BMP format
        return {};
    }

    if (header.bitsPerPixel == 8 && header.coloursUsed == 0)
        header.coloursUsed = 256;

    Array<PixelARGB> colourTable;

    for (int i = 0; i < (int) header.coloursUsed; ++i)
    {
        const auto b = (uint8) input.readByte();
        const auto g = (uint8) input.readByte();
        const auto r = (uint8) input.readByte();
        const auto a = (uint8) input.readByte();

        colourTable.add ({ a, r, g, b });
    }

    const bool isBottomUp = header.height < 0;
    header.height = std::abs (header.height);

    if (! input.setPosition (header.dataOffset))
        return {};

    Image image (Image::ARGB, (int) header.width, (int) header.height, true);
    Image::BitmapData data (image, Image::BitmapData::writeOnly);

    const auto bytesPerPixel = header.bitsPerPixel / 8;
    const auto bytesPerRow = (size_t) (int64) (std::floor ((header.bitsPerPixel * header.width + 31.0) / 32.0) * 4.0);

    HeapBlock<uint8> rowData (bytesPerRow, true);

    for (int y = 0; y < (int) header.height; ++y)
    {
        rowData.clear (bytesPerRow);
        const auto s = input.read (rowData.getData(), bytesPerRow);
        jassertquiet (s == (ssize_t) bytesPerRow);

        for (int x = 0; x < (int) header.width; ++x)
        {
            auto* d = &rowData[x * bytesPerPixel];
            auto* p = (PixelARGB*) data.getPixelPointer (x, (int) (isBottomUp ? y : header.height - y - 1));

            if (! colourTable.isEmpty())
                *p = colourTable[d[0]];
            else
                p->setARGB (bytesPerPixel == 4 ? d[3] : 64, d[2], d[1], d[0]);
        }
    }

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
    for (int y = 0; y < image.getHeight(); ++y)
    {
        for (int x = 0; x < image.getWidth(); ++x)
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
