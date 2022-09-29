void ImageFormatManager::registerFormat (std::unique_ptr<ImageFileFormat> newFormat)
{
    if (newFormat == nullptr || knownFormats.contains (newFormat.get()))
    {
        jassertfalse;
        return;
    }

    for (auto* format : knownFormats)
    {
        if (format->getFormatName().trim().equalsIgnoreCase (newFormat->getFormatName().trim()))
        {
            jassertfalse; // Format has already been registered!
            return;
        }
    }

    knownFormats.add (newFormat.release());
}

void ImageFormatManager::registerBasicFormats()
{
    registerFormat (std::make_unique<JPEGImageFormat>());
    registerFormat (std::make_unique<PNGImageFormat>());
    registerFormat (std::make_unique<GIFImageFormat>());
    registerFormat (std::make_unique<BMPImageFormat>());
    registerFormat (std::make_unique<TGAImageFormat>());

   #if JUCE_MODULE_AVAILABLE_squarepine_images
    registerFormat (std::make_unique<WebPImageFormat>());
   #endif
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

