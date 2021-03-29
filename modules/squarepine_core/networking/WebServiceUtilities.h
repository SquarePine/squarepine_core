/** */
enum class HTTPRequest
{
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE
};

/** */
inline String toString (HTTPRequest type)
{
    if (type == HTTPRequest::GET)           return "GET";
    else if (type == HTTPRequest::HEAD)     return "HEAD";
    else if (type == HTTPRequest::POST)     return "POST";
    else if (type == HTTPRequest::PUT)      return "PUT";
    else if (type == HTTPRequest::DELETE)   return "DELETE";
    else if (type == HTTPRequest::CONNECT)  return "CONNECT";
    else if (type == HTTPRequest::OPTIONS)  return "OPTIONS";
    else if (type == HTTPRequest::TRACE)    return "TRACE";

    jassertfalse; //?
    return "GET";
}

/** */
inline HTTPRequest createHTTPRequestTypeFromString (const String& type)
{
    if (type.compareIgnoreCase ("GET") == 0)            return HTTPRequest::GET;
    else if (type.compareIgnoreCase ("HEAD") == 0)      return HTTPRequest::HEAD;
    else if (type.compareIgnoreCase ("POST") == 0)      return HTTPRequest::POST;
    else if (type.compareIgnoreCase ("PUT") == 0)       return HTTPRequest::PUT;
    else if (type.compareIgnoreCase ("DELETE") == 0)    return HTTPRequest::DELETE;
    else if (type.compareIgnoreCase ("CONNECT") == 0)   return HTTPRequest::CONNECT;
    else if (type.compareIgnoreCase ("OPTIONS") == 0)   return HTTPRequest::OPTIONS;
    else if (type.compareIgnoreCase ("TRACE") == 0)     return HTTPRequest::TRACE;

    jassertfalse; //?
    return HTTPRequest::GET;
}

/** */
inline String toString (const StringPairArray& headers)
{
    String result;

    for (const auto& key : headers.getAllKeys())
        result << key << " " << headers[key] << newLine;

    return result;
}

/** */
inline void setContentType (StringPairArray& headers, const String& type)               { headers.set ("Content-Type:", type); }

/** */
void setContentTypeForFile (StringPairArray& headers, const File& f);

inline void setApplicationContentType (StringPairArray& headers, const String& type)    { setContentType (headers, String ("application/") + type.trim()); }
inline void setContentTypeJson (StringPairArray& headers)                               { setApplicationContentType (headers, "json"); }
inline void setContentTypeZip (StringPairArray& headers)                                { setApplicationContentType (headers, "zip"); }
inline void setContentTypeMsword (StringPairArray& headers)                             { setApplicationContentType (headers, "msword"); }
inline void setContentTypeOggApp (StringPairArray& headers)                             { setApplicationContentType (headers, "ogg"); }
inline void setContentTypePDF (StringPairArray& headers)                                { setApplicationContentType (headers, "pdf"); }
inline void setContentTypeRTF (StringPairArray& headers)                                { setApplicationContentType (headers, "rtf"); }
inline void setContentTypeXMLApplication (StringPairArray& headers)                     { setApplicationContentType (headers, "xml"); }
inline void setContentTypeRAR (StringPairArray& headers)                                { setApplicationContentType (headers, "x-rar-compressed"); }
inline void setContentType7Zip (StringPairArray& headers)                               { setApplicationContentType (headers, "x-7z-compressed"); }
inline void setContentTypeTAR (StringPairArray& headers)                                { setApplicationContentType (headers, "x-tar"); }
inline void setContentTypeJAR (StringPairArray& headers)                                { setApplicationContentType (headers, "java-archive"); }
inline void setContentTypeAppFormEncoded (StringPairArray& headers)                     { setApplicationContentType (headers, "x-www-form-urlencoded"); }

inline void setImageContentType (StringPairArray& headers, const String& type)          { setContentType (headers, String ("image/") + type.trim()); }
inline void setContentTypeBitmap (StringPairArray& headers)                             { setImageContentType (headers, "bmp"); }
inline void setContentTypeGIF (StringPairArray& headers)                                { setImageContentType (headers, "gif"); }
inline void setContentTypeMicrosoftIcon (StringPairArray& headers)                      { setImageContentType (headers, "vnd.microsoft.icon"); }
inline void setContentTypeJPEG (StringPairArray& headers)                               { setImageContentType (headers, "jpeg"); }
inline void setContentTypePNG (StringPairArray& headers)                                { setImageContentType (headers, "png"); }
inline void setContentTypeSVG (StringPairArray& headers)                                { setImageContentType (headers, "svg+xml"); }
inline void setContentTypeTIFF (StringPairArray& headers)                               { setImageContentType (headers, "tiff"); }
inline void setContentTypeWebP (StringPairArray& headers)                               { setImageContentType (headers, "webp"); }

inline void setAudioContentType (StringPairArray& headers, const String& type)          { setContentType (headers, String ("audio/") + type.trim()); }
inline void setContentTypeAAC (StringPairArray& headers)                                { setAudioContentType (headers, "aac"); }
inline void setContentTypeMIDI (StringPairArray& headers)                               { setAudioContentType (headers, "midi"); }
inline void setContentTypeMPEG (StringPairArray& headers)                               { setAudioContentType (headers, "mpeg"); }
inline void setContentTypeOgg (StringPairArray& headers)                                { setAudioContentType (headers, "ogg"); }
inline void setContentTypeWav (StringPairArray& headers)                                { setAudioContentType (headers, "wav"); }
inline void setContentTypeWebM (StringPairArray& headers)                               { setAudioContentType (headers, "webm"); }
inline void setContentType3GPP (StringPairArray& headers)                               { setAudioContentType (headers, "3gpp"); }
inline void setContentType3GPP2 (StringPairArray& headers)                              { setAudioContentType (headers, "3gpp2"); }

inline void setFontContentType (StringPairArray& headers, const String& type)           { setContentType (headers, String ("font/") + type.trim()); }
inline void setContentTypeTTF (StringPairArray& headers)                                { setFontContentType (headers, "ttf"); }
inline void setContentTypeWOFF (StringPairArray& headers)                               { setFontContentType (headers, "woff"); }
inline void setContentTypeWOFF2 (StringPairArray& headers)                              { setFontContentType (headers, "woff2"); }
inline void setContentTypeOTF (StringPairArray& headers)                                { setFontContentType (headers, "otf"); }

inline void setTextContentType (StringPairArray& headers, const String& type)           { setContentType (headers, String ("text/") + type.trim()); }
inline void setContentTypePlain (StringPairArray& headers)                              { setTextContentType (headers, "plain"); }
inline void setContentTypeCSS (StringPairArray& headers)                                { setTextContentType (headers, "css"); }
inline void setContentTypeCSV (StringPairArray& headers)                                { setTextContentType (headers, "csv"); }
inline void setContentTypeHTML (StringPairArray& headers)                               { setTextContentType (headers, "html"); }
inline void setContentTypeCalendar (StringPairArray& headers)                           { setTextContentType (headers, "calendar"); }
inline void setContentTypeJavascript (StringPairArray& headers)                         { setTextContentType (headers, "javascript"); }
inline void setContentTypeXMLText (StringPairArray& headers)                            { setTextContentType (headers, "xml"); }

String createFilteredUrl (String domain, String path);

std::unique_ptr<WebInputStream> createEndpointStream (const URL& url,
                                                      HTTPRequest requestType,
                                                      const StringPairArray& headers = {},
                                                      int timeoutMs = 5000);
