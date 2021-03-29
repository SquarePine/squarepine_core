//==============================================================================
TGAImageFormat::TargaPixel::TargaPixel (uint8 b_, uint8 g_, uint8 r_, uint8 a_) noexcept :
    b (b_),
    g (g_),
    r (r_),
    a (a_)
{
}

TGAImageFormat::TargaPixel::TargaPixel (uint8 bgr, uint8 a_) noexcept :
    TargaPixel (bgr, bgr, bgr, a_)
{
}

//==============================================================================
class TGAImageFormat::Helpers
{
public:
    /** Truncates an int value to that of an int16 */
    static inline int16 toInt16 (int value) noexcept
    {
        return static_cast<int16> (std::clamp (value, -32767, 32767));
    }

    /** Set the header's bounds, as per a rectangle */
    static void setBounds (TargaHeader& header, Rectangle<int> bounds)
    {
        header.originX = toInt16 (bounds.getX());
        header.originY = toInt16 (bounds.getY());
        header.width   = toInt16 (bounds.getWidth());
        header.height  = toInt16 (bounds.getHeight());
    }

    /** One place to take care of for header format reading and writing
        to follow the same pattern, through and through
    */
    template<class StreamType, typename ReturnType, typename BufferType, typename SizeFormat,
             ReturnType (StreamType::*ReadOrWriteMethod) (BufferType, SizeFormat)>
    static void performHeaderIO (TargaHeader& header, StreamType& stream)
    {
        stream.setPosition (0);

        (stream.*ReadOrWriteMethod) (&header.idLength,          1);
        (stream.*ReadOrWriteMethod) (&header.colourMapType,     1);
        (stream.*ReadOrWriteMethod) (&header.dataTypeCode,      1);
        (stream.*ReadOrWriteMethod) (&header.colourMapOrigin,   2);
        (stream.*ReadOrWriteMethod) (&header.colourMapLength,   2);
        (stream.*ReadOrWriteMethod) (&header.colourMapDepth,    1);
        (stream.*ReadOrWriteMethod) (&header.originX,           2);
        (stream.*ReadOrWriteMethod) (&header.originY,           2);
        (stream.*ReadOrWriteMethod) (&header.width,             2);
        (stream.*ReadOrWriteMethod) (&header.height,            2);
        (stream.*ReadOrWriteMethod) (&header.bitsPerPixel,      1);
        (stream.*ReadOrWriteMethod) (&header.imageDescriptor,   1);
    }

    /** Read image pixel data, without alpha */
    static inline void readPixelsToBitmapData (const Image::BitmapData& bitmapData,
                                               HeapBlock<TargaPixel>& pixels)
    {
        int n = 0;

        for (int y = bitmapData.height; --y >= 0;)
        {
            const auto* line = bitmapData.getLinePointer (y);

            for (int i = bitmapData.width; --i >= 0;)
            {
                ((PixelRGB*) line)->setARGB (0, pixels[n].r, pixels[n].g, pixels[n].b);

                line += bitmapData.pixelStride;
                ++n;
            }
        }
    }

    /** Read image pixel data, with alpha */
    static inline void readPixelsToBitmapDataWithAlpha (const Image::BitmapData& bitmapData,
                                                        HeapBlock<TargaPixel>& pixels)
    {
        int n = 0;

        for (int y = bitmapData.height; --y >= 0;)
        {
            const auto* line = bitmapData.getLinePointer (y);

            for (int i = 0; i < bitmapData.width; ++i)
            {
                ((PixelARGB*) line)->setARGB (pixels[n].a, pixels[n].r, pixels[n].g, pixels[n].b);
                ((PixelARGB*) line)->premultiply();

                line += bitmapData.pixelStride;
                ++n;
            }
        }
    }

private:
    SQUAREPINE_DECLARE_TOOL_CLASS (Helpers)
};

//==============================================================================
class TGAImageFormat::TargaReader
{
public:
    /** Reads pixel data from the stream, converting such to 24-bit or 32-bit data */
    static bool readPixelsFromStream (HeapBlock<TargaPixel>& pixels,
                                      InputStream& stream,
                                      int numBytes,
                                      TargaType format,
                                      const TargaHeader& header)
    {
        switch (format)
        {
            case uncompressedColourMapped:
                readUncompressedColourMapped (pixels, stream, numBytes);
            break;

            case uncompressedRGB:
                readUncompressedRGB (pixels, stream, numBytes, header);
            break;

            case uncompressedBlackWhite:
                readUncompressedBlackWhite (pixels, stream, numBytes);
            break;

            case rleColourMapped:
            case rleRGB:
            case rleBlackWhite:
            case compressedColourMapped:
            case compressedColourMapped4Pass:
                return false;

            default:
                jassertfalse; //Format is unknown!
                return false;
        };

        return true;
    }

    static forcedinline uint8 readUncompressedChannel (InputStream& stream)
    {
        uint8 channel = 0;
        stream.read (&channel, 1);
        return channel;
    }

    static forcedinline int extractBit (int number, int k, int p)
    {
        return (((1 << k) - 1) & (number >> (p - 1)));
    }

    static forcedinline void readUncompressedColourMapped (HeapBlock<TargaPixel>& pixels,
                                                           InputStream& stream,
                                                           int numBytes)
    {
        stream.read (pixels, numBytes);
    }

/*
    switch( bpp_in )
    {
    case 32:
        if( alphabits == 0 )
            goto is_24_bit_in_disguise;

        // 32-bit to 32-bit -- nop.
        break;

    case 24:
is_24_bit_in_disguise:
        // 24-bit to 32-bit; (only force alpha to full)
        pixel |= 0xFF000000;
        break;

    case 15:
is_15_bit_in_disguise:
        r = (ubyte)(((float)((pixel & 0x7C00) >> 10)) * 8.2258f);
        g = (ubyte)(((float)((pixel & 0x03E0) >> 5 )) * 8.2258f);
        b = (ubyte)(((float)(pixel & 0x001F)) * 8.2258f);
        // 15-bit to 32-bit; (force alpha to full)
        pixel = 0xFF000000 + (r << 16) + (g << 8) + b;
        break;
        
    case 16:
        if( alphabits == 1 )
            goto is_15_bit_in_disguise;

        // 16-bit to 32-bit; (force alpha to full)
        r = (ubyte)(((float)((pixel & 0xF800) >> 11)) * 8.2258f);
        g = (ubyte)(((float)((pixel & 0x07E0) >> 5 )) * 4.0476f);
        b = (ubyte)(((float)(pixel & 0x001F)) * 8.2258f);
        pixel = 0xFF000000 + (r << 16) + (g << 8) + b;
        break;
    }
*/

    static forcedinline void readUncompressedRGB15bit (TargaPixel&, InputStream&)
    {
    }

    static forcedinline void readUncompressedRGB16bit (TargaPixel&, InputStream&)
    {
    }

    static void readUncompressedRGB (HeapBlock<TargaPixel>& pixels,
                                     InputStream& stream,
                                     int numBytes,
                                     const TargaHeader& header)
    {
        if (header.bitsPerPixel != 32 && header.bitsPerPixel != 24
            && header.bitsPerPixel != 16 && header.bitsPerPixel != 15)
        {
            jassertfalse; //Broken or corrupt data...
            return;
        }

        const auto bytesPerPixel = header.bitsPerPixel / 8;

        auto* iterator = pixels.getData();

        int i = 0;
        while (numBytes > 0)
        {
            if (bytesPerPixel == 2)
            {
                //The 2 byte entry is broken down as follows,
                //where each letter represents a bit:
                //ARRRRRGG GGGBBBBB

                //const auto arg = readUncompressedChannel (stream);
                //const auto gb = readUncompressedChannel (stream);

                iterator[i].b = 0;
                iterator[i].g = 0;
                iterator[i].r = 0;
                iterator[i].a = 0;
            }
            else
            {
                iterator[i].b = readUncompressedChannel (stream);
                iterator[i].g = readUncompressedChannel (stream);
                iterator[i].r = readUncompressedChannel (stream);
                iterator[i].a = bytesPerPixel == 4 ? readUncompressedChannel (stream) : 1;
            }

            numBytes -= bytesPerPixel;
            ++i;
        }
    }

    static void readUncompressedBlackWhite (HeapBlock<TargaPixel>& pixels,
                                            InputStream& stream,
                                            int numBytes)
    {
        HeapBlock<uint8> greyscaleChannel ((size_t) numBytes);
        const auto numRead = stream.read (greyscaleChannel, numBytes);

        auto* const iterator = pixels.getData();
        auto* channel = greyscaleChannel.getData();

        for (int i = numRead; --i >= 0;)
            iterator[i] = TargaPixel (channel[i]);
    }

private:
    SQUAREPINE_DECLARE_TOOL_CLASS (TargaReader)
};

//==============================================================================
bool TGAImageFormat::canUnderstand (InputStream& stream)
{
    readHeader (header, stream);

    //Find out if we support the given BPP:
    switch (header.bitsPerPixel)
    {
        case 8:
        case 15:
        case 16:
        case 24:
        case 32:
            break;

        default:
            return false;
    };

    //Find out if we support the given type of colour map:
    switch (header.colourMapType)
    {
        case 0:
        case 1:
            break;

        default:
            return false;
    };

    //Find out if we support the current type of image:
    switch (header.dataTypeCode)
    {
        case uncompressedRGB:
        case uncompressedColourMapped:
        case uncompressedBlackWhite:
            break;

        default:
            return false;
    };

    return true;
}

Image TGAImageFormat::decodeImage (InputStream& stream)
{
    if (! canUnderstand (stream))
        return {};

    //Skip over unnecessary stuff:
    const auto skipOver = header.idLength + (header.colourMapType * header.colourMapLength);

    stream.setPosition (stream.getPosition() + skipOver); //NB: Faster than calling skipBytes.

    //Calculate the amount of space needed for the image:
    const auto onePixelSize = header.bitsPerPixel / 8;
    const auto imageSize = header.width * header.height * onePixelSize;

    //Read the image data:
    HeapBlock<TargaPixel> pixels ((size_t) imageSize, true);

    if (TargaReader::readPixelsFromStream (pixels, stream, imageSize, (TargaType) header.dataTypeCode, header))
    {
        const bool hasAlphaChannel = (header.bitsPerPixel / 8) == 4;

        Image image (hasAlphaChannel ? Image::ARGB : Image::RGB,
                     (int) header.width, (int) header.height, hasAlphaChannel);

        image.getProperties()->set ("originalImageHadAlpha", hasAlphaChannel);

        const Image::BitmapData destData (image, Image::BitmapData::writeOnly);

        if (hasAlphaChannel)
            Helpers::readPixelsToBitmapDataWithAlpha (destData, pixels);
        else
            Helpers::readPixelsToBitmapData (destData, pixels);

        return image;
    }

    return {};
}

bool TGAImageFormat::writeImageToStream (const Image& image, OutputStream& stream)
{
    jassertfalse; //Incomplete!
    Helpers::setBounds (header, image.getBounds());
    writeHeader (stream, header);

    return false;
}

//==============================================================================
void TGAImageFormat::readHeader (TargaHeader& header, InputStream& stream)
{
    Helpers::performHeaderIO<InputStream, int, void*, int, &InputStream::read> (header, stream);
}

void TGAImageFormat::writeHeader (OutputStream& stream, TargaHeader& header)
{
    Helpers::performHeaderIO<OutputStream, bool, const void*, size_t, &OutputStream::write> (header, stream);
}
