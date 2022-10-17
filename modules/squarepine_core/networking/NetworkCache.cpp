//==============================================================================
namespace networking
{
constexpr auto networkCacheCheckIntervalMs = 5000;

StringArray splitAtCommas (const String& source)
{
    auto values = StringArray::fromTokens (source, ",", "\"");
    values.trim();
    values.removeEmptyStrings();
    values.removeDuplicates (true);
    values.minimiseStorageOverheads();
    return values;
}

void parsePragmaValue (NetworkCacheConfiguration& c, const String& value)
{
    for (const auto& it: splitAtCommas (value))
        if (it.containsIgnoreCase ("no-cache"))
            c.maxAge = 0;
}

int64 parseEpochFromSingleOrISO8601 (const String& value)
{
    int64 result = 0;
    std::istringstream iss (value.toStdString());
    iss >> result;
    if (! iss.fail())
        return result;

    return Time::fromISO8601 (value).toMilliseconds();
}

void parseExpiry (NetworkCacheConfiguration& c, const String& value) { c.expiry = parseEpochFromSingleOrISO8601 (value); }
void parseAge (NetworkCacheConfiguration& c, const String& value) { c.age = parseEpochFromSingleOrISO8601 (value); }
void parseMaxAge (NetworkCacheConfiguration& c, const String& value) { c.maxAge = parseEpochFromSingleOrISO8601 (value); }
void parseResponseDate (NetworkCacheConfiguration& c, const String& value) { c.responseDate = parseEpochFromSingleOrISO8601 (value); }
void parseLastModified (NetworkCacheConfiguration& c, const String& value) { c.lastModifiedDate = parseEpochFromSingleOrISO8601 (value); }
void parseContentType (NetworkCacheConfiguration& c, const String& value) { c.isContentText = ! value.containsIgnoreCase ("image"); }

void parseCacheControlValue (NetworkCacheConfiguration& c, const String& value)
{
    for (const auto& it: splitAtCommas (value))
    {
        if (it.containsIgnoreCase ("max-age"))
            parseMaxAge (c, it.substring (it.indexOf ("=") + 1));
        else if (it.containsIgnoreCase ("no-store"))
            c.type = CacheControlType::noStore;
        else if (it.containsIgnoreCase ("no-cache"))
            c.type = CacheControlType::noCache;
        else if (it.containsIgnoreCase ("private"))
            c.type = CacheControlType::isPrivate;
        else if (it.containsIgnoreCase ("public"))
            c.type = CacheControlType::isPublic;
    }
}

void parseHeader (NetworkCacheConfiguration& c, const String& key, const String& value)
{
    if (key.equalsIgnoreCase ("Cache-Control"))
        parseCacheControlValue (c, value);
    else if (key.equalsIgnoreCase ("Pragma"))
        parsePragmaValue (c, value);
    else if (key.equalsIgnoreCase ("Expires"))
        parseExpiry (c, value);
    else if (key.equalsIgnoreCase ("Content-Type"))
        parseContentType (c, value);
    else if (key.equalsIgnoreCase ("Age"))
        parseAge (c, value);
    else if (key.equalsIgnoreCase ("Last-Modified"))
        parseLastModified (c, value);
    else if (key.equalsIgnoreCase ("Date"))
        parseResponseDate (c, value);
    else if (key.equalsIgnoreCase ("Content-Length"))
        c.contentLength = value.getLargeIntValue();
    else if (key.equalsIgnoreCase ("ETag"))
        c.etag = value.trim();
    else if (key.equalsIgnoreCase ("Content-Encoding"))
        c.contentEncoding = value.trim();
}

void parseHeaders (NetworkCacheConfiguration& c, const StringPairArray& headers)
{
    const auto& k = headers.getAllKeys();
    const auto& v = headers.getAllValues();

    for (int i = 0; i < k.size(); ++i)
        parseHeader (c, k.strings.getUnchecked (i), v.strings.getUnchecked (i));
}

//==============================================================================
File createCacheDirectory()
{
    return File::getSpecialLocation (File::SpecialLocationType::commonApplicationDataDirectory)
        .getChildFile (File::createLegalFileName (JUCEApplication::getInstance()->getApplicationName()))
        .getChildFile ("NetworkCache");
}

/** @warning @todo This easily blows the Windows MAX_PATH limit of 260 chars. */
File createCacheFile (const URL& url)
{
    return createCacheDirectory()
        .getChildFile (juce::SHA256 (url.toString (true).toUTF8()).toHexString());
}
}

//==============================================================================
JUCE_IMPLEMENT_SINGLETON (NetworkCache)

NetworkResponse::NetworkResponse (const URL& sourceUrl): url (sourceUrl)
{
}

NetworkResponse::~NetworkResponse()
{
}

void NetworkResponse::reset()
{
    cache = {};
    responseHeaders = {};
    body.reset();
    storedLocation = File();
}

bool NetworkResponse::fetch()
{
    SQUAREPINE_CRASH_TRACER

    if (! NetworkConnectivityChecker().isConnectedToInternet())
        return false;

    const auto f = networking::createCacheFile (url);

    if (stream != nullptr)
    {
        if (! isExpired())
            return true;

        if (f.exists())
        {
            jassertfalse;// This is probably wrong and needs to be dynamically assessed somehow...
        }

        if (storedLocation.existsAsFile())
            storedLocation.deleteRecursively (true);
    }

    reset();// This is also probably wrong...

    if (auto s = url.createInputStream (URL::InputStreamOptions (URL::ParameterHandling::inAddress)))
        stream.reset (dynamicCastUniquePtr<WebInputStream> (std::unique_ptr<InputStream> (s.release())).release());
    else
        stream.reset();

    cache.responseDate = getNow();
    if (stream == nullptr)
    {
        jassertfalse;
        return false;
    }

    responseHeaders = stream->getResponseHeaders();
    responseHeaders.minimiseStorageOverheads();

    networking::parseHeaders (cache, responseHeaders);
    stream->readIntoMemoryBlock (body);

    if (supportsCaching())
    {
        if (! f.exists() && f.create().failed())
        {
            jassertfalse;
            return false;
        }

        storedLocation = f;

        FileOutputStream fos (storedLocation);
        fos << body;
        fos.flush();
    }

    return true;
}

const StringPairArray& NetworkResponse::getResponseHeaders() const
{
    jassert (stream != nullptr);// You didn't fetch the stream data yet...
    return responseHeaders;
}

const MemoryBlock& NetworkResponse::getBody() const
{
    jassert (stream != nullptr);// You didn't fetch the stream data yet...
    return body;
}

bool NetworkResponse::supportsCaching() const
{
    jassert (stream != nullptr);// You didn't fetch the stream data yet...
    return stream != nullptr
           && cache.type.canBeCachedAndStored();
}

int64 NetworkResponse::getNow()
{
    return Time::getCurrentTime().toMilliseconds();
}

bool NetworkResponse::isExpired() const
{
    SQUAREPINE_CRASH_TRACER

    if (! supportsCaching())
        return false;

    /** RFC 7234 section 4.2.3:
        apparentAge
            is the value of Age: header received by the cache with this response.
        date_value
            is the value of the origin server's Date: header
        request_time
            is the (local) time when the cache made the request that resulted in this cached response
        response_time
            is the (local) time when the cache received the response
        now
            is the current (local) time
    */
    const auto apparentAge = jmax ((int64) 0, cache.responseDate - cache.requestDate);
    const auto correctedReceivedAge = jmax (apparentAge, cache.age);
    const auto responseDelay = cache.responseDate - cache.requestDate;
    const auto correctedInitialAge = correctedReceivedAge + responseDelay;
    const auto residentTime = getNow() - cache.responseDate;
    const auto currentAge = correctedInitialAge + residentTime;

    return currentAge < cache.maxAge;
}

//==============================================================================
NetworkCache::NetworkCache(): Thread ("Networking Thread")
{
    startThread();
}

NetworkCache::~NetworkCache()
{
    clearSingletonInstance();
}

void NetworkCache::shutdown()
{
    if (auto* cache = NetworkCache::getInstanceWithoutCreating())
        shutdownThreadSafely (*cache);
}

void NetworkCache::enqueue (const URL& url, std::weak_ptr<ResponseCallback>)
{
    if (! url.isWellFormed() || url.isLocalFile())
        return;

    for (int i = responses.size(); --i >= 0;)
        if (auto existingResponse = responses.getUnchecked (i))
            if (existingResponse->getSourceURL() == url)
                return;

    responses.add (new NetworkResponse (url));
}

void NetworkCache::purgeIfExpired (NetworkResponse::Ptr response)
{
    purgeInternal (response, true, true);
}

void NetworkCache::purgeInternal (NetworkResponse::Ptr response, bool mustCheckForExpiry, bool mustCheckIfExists)
{
    if (response == nullptr)
    {
        jassertfalse;
        return;
    }

    if (mustCheckIfExists && ! responses.contains (response))
        return;

    if (mustCheckForExpiry && ! response->isExpired())
        return;

    responses.removeObject (response);
    response->getStoredLocation().deleteRecursively (true);
}

void NetworkCache::purgeCache()
{
    auto dir = networking::createCacheDirectory();
    if (dir.exists())
        dir.deleteRecursively (true);

    responses.clearQuick();
}

void NetworkCache::run()
{
    for (;;)
    {
        const NetworkConnectivityChecker checker;
        if (! checker.isConnectedToInternet())
            continue;

        for (int i = 0; i < responses.size(); ++i)
            purgeInternal (responses.getUnchecked (i), true, false);
    }
}
