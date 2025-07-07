void ImageFormatManager::registerFormat (std::unique_ptr<ImageFileFormat> newFormat,
                                         const String& extensionWildcard)
{
    const auto newWildcard = extensionWildcard.trim();

    if (newFormat == nullptr
        || newWildcard.isEmpty()
        || wildcards.contains (newWildcard)
        || knownFormats.contains (newFormat.get()))
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
    wildcards.add (extensionWildcard);
}

void ImageFormatManager::registerBasicFormats()
{
    registerFormat (std::make_unique<JPEGImageFormat>(), "jpeg;jpg");
    registerFormat (std::make_unique<PNGImageFormat>(), "png");
    registerFormat (std::make_unique<GIFImageFormat>(), "gif");
    registerFormat (std::make_unique<BMPImageFormat>(), "bmp;dib");
    registerFormat (std::make_unique<TGAImageFormat>(), "tga");

   #if JUCE_MODULE_AVAILABLE_squarepine_images
    registerFormat (std::make_unique<WebPImageFormat>(), "webp");
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
String ImageFormatManager::getWildcardForAllFormats (bool sorted) const
{
    StringArray extensions;

/*
    for (const auto& wc : wildcards)
    {
        if (wc.contains (";"))
        {
            for (const auto& tok : StringArray::fromTokens (wc, ";", ""))
            {
            }
        }

        String c = wc;
        if (! e.startsWithChar ('.') && ! e.startsWithChar ('.'))
            c = "";

        
    }
*/
    for (auto& e : extensions)
        if (! e.startsWithChar ('.') && ! e.startsWithChar ('.'))
            e = "." + e;

    extensions.trim();
    extensions.removeEmptyStrings();

    for (auto& e : extensions)
        e = (e.startsWithChar ('.') ? "*" : "*.") + e;

    extensions.removeDuplicates (true);

    if (sorted)
        extensions.sortNatural();

    return extensions.joinIntoString (";");
}

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

Image ImageFormatManager::loadFrom (const URL& url)
{
    if (url.isLocalFile())
        return loadFrom (url.getLocalFile());

    return {};
}

Image ImageFormatManager::loadFrom (const AndroidDocument& androidDocument)
{
    const auto info = androidDocument.getInfo();
    if (info.isFile() && info.canRead())
    {
        const auto mimeType = info.getType();
        if (mimeType.isNotEmpty())
            if (! mimeType.containsIgnoreCase ("image"))
                return {};

        return loadFrom (androidDocument.getUrl());
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

