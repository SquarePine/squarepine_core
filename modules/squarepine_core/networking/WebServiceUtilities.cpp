void setContentTypeForFile (StringPairArray& h, const File& f)
{
    if (f.hasFileExtension ("doc;docx;docm;docb;dot;dotx;dotm;wbk"))   { setContentTypeMsword (h); }
    else if (f.hasFileExtension ("xml"))        { setContentTypeXMLText (h); }
    else if (f.hasFileExtension ("json"))       { setContentTypeJson (h); }
    else if (f.hasFileExtension ("zip"))        { setContentTypeZip (h); }
    else if (f.hasFileExtension ("pdf"))        { setContentTypePDF (h); }
    else if (f.hasFileExtension ("rtf"))        { setContentTypeRTF (h); }
    else if (f.hasFileExtension ("rar"))        { setContentTypeRAR (h); }
    else if (f.hasFileExtension ("7z"))         { setContentType7Zip (h); }
    else if (f.hasFileExtension ("tar"))        { setContentTypeTAR (h); }
    else if (f.hasFileExtension ("jar"))        { setContentTypeJAR (h); }
    else if (f.hasFileExtension ("bmp"))        { setContentTypeBitmap (h); }
    else if (f.hasFileExtension ("gif"))        { setContentTypeGIF (h); }
    else if (f.hasFileExtension ("ico"))        { setContentTypeMicrosoftIcon (h); }
    else if (f.hasFileExtension ("jpg;jpeg"))   { setContentTypeJPEG (h); }
    else if (f.hasFileExtension ("png"))        { setContentTypePNG (h); }
    else if (f.hasFileExtension ("svg"))        { setContentTypeSVG (h); }
    else if (f.hasFileExtension ("tiff"))       { setContentTypeTIFF (h); }
    else if (f.hasFileExtension ("webp"))       { setContentTypeWebP (h); }
    else if (f.hasFileExtension ("aac"))        { setContentTypeAAC (h); }
    else if (f.hasFileExtension ("mid;midi"))   { setContentTypeMIDI (h); }
    else if (f.hasFileExtension ("mpg;mpeg"))   { setContentTypeMPEG (h); }
    else if (f.hasFileExtension ("ogg"))        { setContentTypeOgg (h); }
    else if (f.hasFileExtension ("wav"))        { setContentTypeWav (h); }
    else if (f.hasFileExtension ("webm"))       { setContentTypeWebM (h); }
    else if (f.hasFileExtension ("3gp"))        { setContentType3GPP (h); }
    else if (f.hasFileExtension ("3g2"))        { setContentType3GPP2 (h); }
    else if (f.hasFileExtension ("ttf"))        { setContentTypeTTF (h); }
    else if (f.hasFileExtension ("woff"))       { setContentTypeWOFF (h); }
    else if (f.hasFileExtension ("woff2"))      { setContentTypeWOFF2 (h); }
    else if (f.hasFileExtension ("otf"))        { setContentTypeOTF (h); }
    else if (f.hasFileExtension ("txt"))        { setContentTypePlain (h); }
    else if (f.hasFileExtension ("css"))        { setContentTypeCSS (h); }
    else if (f.hasFileExtension ("csv"))        { setContentTypeCSV (h); }
    else if (f.hasFileExtension ("htm;html"))   { setContentTypeHTML (h); }
    else if (f.hasFileExtension ("ics;ifb"))    { setContentTypeCalendar (h); }
    else if (f.hasFileExtension ("js"))         { setContentTypeJavascript (h); }
    else
    {
        jassertfalse; // Unknown extension
    }
}

String createFilteredUrl (String domain, String path)
{
    domain = domain.trim();
    if (domain.isEmpty())
    {
        jassertfalse;
        return {};
    }

    if (! domain.startsWithIgnoreCase ("http"))
        domain = "https" + domain;

    path = path.trim();
    if (path.isEmpty())
    {
        jassertfalse;
        return {};
    }

    if (! domain.endsWithIgnoreCase ("/") && ! path.startsWithIgnoreCase ("/"))
        domain << "/";

    return domain + path;
}

std::unique_ptr<WebInputStream> createEndpointStream (const URL& url,
                                                      HTTPRequest requestType,
                                                      const StringPairArray& headers,
                                                      int timeoutMs)
{
    jassert (url.isWellFormed());

    const auto formattedHeaders = networking::createUserAgentHeader() + toString (headers);

    auto timeSec = -1.0;

   #if SQUAREPINE_LOG_NETWORK_CALLS
    Logger::writeToLog (String ("Performing network call with the following:") + newLine
                        + "\tURL:" + newLine + url.toString (true) + newLine
                        + "\tHeaders:" + newLine + formattedHeaders);
   #endif

    bool wasStreamValid = false;

    {
        const ScopedTimeMeasurement stm (timeSec);

        auto s = url.createInputStream (URL::InputStreamOptions (URL::ParameterHandling::inAddress)
                                        .withHttpRequestCmd (toString (requestType))
                                        .withExtraHeaders (formattedHeaders)
                                        .withNumRedirectsToFollow (10)
                                        .withConnectionTimeoutMs (timeoutMs));

        if (auto stream = dynamicCastUniquePtr<WebInputStream> (std::move (s)))
        {
           #if SQUAREPINE_LOG_NETWORK_CALLS
            Logger::writeToLog (String ("Network call completed:") + newLine
                                + "\tStatus code: " + String (stream->getStatusCode()) + newLine
                                + "\tResponse headers: " + newLine + toString (stream->getResponseHeaders()) + newLine);
           #endif

            if (! stream->isError() && stream->getStatusCode() == 200)
                return stream;

            wasStreamValid = true;

           #if SQUAREPINE_LOG_NETWORK_CALLS
            Logger::writeToLog (String ("\tFailure response code:\t") + String (stream->getStatusCode()) + newLine);

            auto body = stream->readEntireStreamAsString();
            beautifyJSON (body);
            Logger::writeToLog (String ("\tFailure response body:") + newLine + body + newLine);
           #endif
        }
    }

    if (timeSec < 0.0 && NetworkConnectivityChecker().isConnectedToInternet())
    {
        jassertfalse; // JUCE screwed something up here probably...
    }
    else if ((timeSec * 1000.0) >= (double) timeoutMs)
    {
       #if SQUAREPINE_LOG_NETWORK_CALLS
        Logger::writeToLog ("Operation timed out...");
       #endif

        jassertfalse;
    }
    else
    {
        if (wasStreamValid)
        {
           #if SQUAREPINE_LOG_NETWORK_CALLS
            Logger::writeToLog ("Stream was valid. Check logs for error contents.");
           #endif
        }
        else
        {
            jassertfalse; // Some kind of unexpected stream failure occurred.
                          // If you're logging network calls, check the output window for more details.
        }
    }

    return {};
}
