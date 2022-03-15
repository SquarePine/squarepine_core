void ImageFormatManager::registerFormat (ImageFileFormat* newFormat)
{
    if (newFormat == nullptr || knownFormats.contains (newFormat))
    {
        jassertfalse;
        return;
    }

    for (auto* format : knownFormats)
    {
        if (format->getFormatName().trim().equalsIgnoreCase (newFormat->getFormatName().trim()))
        {
            std::unique_ptr<ImageFileFormat> deleter (newFormat);
            newFormat = nullptr;
            jassertfalse; //Format has already been registered!
            return;
        }
    }

    knownFormats.add (newFormat);
}

void ImageFormatManager::registerBasicFormats()
{
    registerFormat (new JPEGImageFormat());
    registerFormat (new PNGImageFormat());
    registerFormat (new GIFImageFormat());
    registerFormat (new BMPImageFormat());
    registerFormat (new TGAImageFormat());
}

void ImageFormatManager::clearFormats()
{
    knownFormats.clear();
}

//==============================================================================
ImageFileFormat* ImageFormatManager::findFormatForStream (InputStream& input)
{
    const auto originalStreamPos = input.getPosition();

    for (auto* format : knownFormats)
    {
        const bool canUnderstand = format->canUnderstand (input);

        input.setPosition (originalStreamPos);

        if (canUnderstand)
            return format;
    }

    return nullptr;
}

ImageFileFormat* ImageFormatManager::findFormatForFile (const File& file) const
{
    if (file.existsAsFile())
        for (auto* format : knownFormats)
            if (format->usesFileExtension (file))
                return format;

    return nullptr;
}

//==============================================================================
Image ImageFormatManager::loadFrom (InputStream& input)
{
    if (auto* const format = findFormatForStream (input))
        return format->decodeImage (input);

    return {};
}

Image ImageFormatManager::loadFrom (const File& file)
{
    FileInputStream stream (file);

    if (stream.openedOk())
    {
        BufferedInputStream bis (stream, 1 << 20);
        return loadFrom (stream);
    }

    return {};
}

Image ImageFormatManager::loadFrom (const void* rawData, const size_t numBytes)
{
    if (rawData != nullptr && numBytes > 4)
    {
        MemoryInputStream mis (rawData, numBytes, false);
        return loadFrom (mis);
    }

    return {};
}

//==============================================================================
Image ImageFormatManager::fromBase64 (const String& data)
{
    if (data.isNotEmpty())
    {
        MemoryOutputStream mos;
        if (Base64::convertFromBase64 (mos, data))
            return loadFrom (mos.getData(), mos.getDataSize());
    }

    return {};
}

String ImageFormatManager::toBase64 (const Image& image)
{
    if (image.isValid())
    {
        for (auto* format : knownFormats)
        {
            MemoryOutputStream mos;
            if (format != nullptr && format->writeImageToStream (image, mos))
                return Base64::toBase64 (mos.getData(), mos.getDataSize());
        }
    }

    return {};
}

