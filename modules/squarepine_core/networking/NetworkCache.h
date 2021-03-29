//==============================================================================
/**
    @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#Cacheability
    @see https://developers.google.com/web/fundamentals/performance/optimizing-content-efficiency/http-caching
*/
class CacheControlType
{
public:
    /** Create a non-cacheable and non-storable cache control. */
    CacheControlType() = default;

    /** Creates a CacheControlType object using a combination of flags from the Flags enum. */
    CacheControlType (int cacheControlTypeFlags) noexcept : flags (cacheControlTypeFlags) { }

    /** Creates a copy of another CacheControlType object. */
    CacheControlType (const CacheControlType&) = default;

    /** Destructor. */
    ~CacheControlType() = default;

    /** Copies another CacheControlType object. */
    CacheControlType& operator= (const CacheControlType&) = default;

    bool operator== (const CacheControlType& other) const noexcept  { return flags == other.flags; }
    bool operator!= (const CacheControlType& other) const noexcept  { return flags != other.flags; }

    //==============================================================================
    /** @returns the raw flags that are set for this network cache control. */
    int getFlags() const noexcept { return flags; }

    /** Tests a set of flags for this object.
        @returns true if any of the flags passed in are set on this object.
    */
    bool testFlags (int flagsToTest) const noexcept { return (flags & flagsToTest) != 0; }

    bool canBeCachedAndStored() const noexcept
    {
        return ! testFlags (CacheControlType::noStore) && ! testFlags (CacheControlType::noCache);
    }

    //==============================================================================
    /** Flag values that can be combined and used in the constructor. */
    enum Flags
    {
        noStore     = 1,
        noCache     = 2,
        isPublic    = 4,
        isPrivate   = 8
    };

private:
    //==============================================================================
    int flags = CacheControlType::noStore | CacheControlType::noCache;
};

//==============================================================================
struct NetworkCacheConfiguration
{
    int64 age = 0, maxAge = 0, expiry = 0, contentLength = 0;
    int64 requestDate = 0, responseDate = 0, lastModifiedDate = 0;
    bool isContentText = false;
    CacheControlType type;
    String etag, contentEncoding;
};

//==============================================================================
class NetworkResponse final : public ReferenceCountedObject
{
public:
    //==============================================================================
    /** The class is reference-counted, so this is a handy pointer class for it. */
    using Ptr = ReferenceCountedObjectPtr<NetworkResponse>;

    //==============================================================================
    /** Assumes a valid URL is provided. */
    NetworkResponse (const URL& sourceUrl);

    ~NetworkResponse();

    bool fetch();

    const URL& getSourceURL() const noexcept { return url; }

    bool hasDownloaded() const noexcept { return stream != nullptr; }
    const MemoryBlock& getBody() const;
    String getContentLength() const;

    const StringPairArray& getResponseHeaders() const;

    /**
        @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Caching
    */
    bool supportsCaching() const;

    /** @returns the network cache configuration for this response. */
    const NetworkCacheConfiguration& getCacheConfiguration() const noexcept { return cache; }

    /** @returns the current epoch time. */
    static int64 getNow();

    /** @returns true if the response is expired.
        The response will not be expired if it doesn't support caching.
    */
    bool isExpired() const;

    const File& getStoredLocation() const { return storedLocation; }

private:
    //==============================================================================
    const URL url;
    std::unique_ptr<WebInputStream> stream;
    NetworkCacheConfiguration cache;
    StringPairArray responseHeaders;
    MemoryBlock body;
    File storedLocation;

    void reset();

    //==============================================================================
    NetworkResponse() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NetworkResponse)
};

//==============================================================================
class NetworkCache : public DeletedAtShutdown,
                     public Thread
{
public:
    NetworkCache();
    ~NetworkCache() override;

    //==============================================================================
    static void shutdown();

    //==============================================================================
    using ResponseCallback = std::function<void (int, NetworkResponse::Ptr)>;

    void enqueue (const URL& url, std::weak_ptr<ResponseCallback> callback = {});

    void purgeCache();

    //==============================================================================
    /** @internal */
    void run() override;

    //==============================================================================
    JUCE_DECLARE_SINGLETON (NetworkCache, false)

private:
    //==============================================================================
    ReferenceCountedArray<NetworkResponse, SpinLock> responses;

    void purgeIfExpired (NetworkResponse::Ptr);
    void purge (NetworkResponse::Ptr);
    void purgeInternal (NetworkResponse::Ptr, bool mustCheckForExpiry, bool mustCheckIfExists);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NetworkCache)
};

