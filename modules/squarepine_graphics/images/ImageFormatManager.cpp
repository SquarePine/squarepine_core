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
        BufferedInputStream bis (stream, 8192);
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
