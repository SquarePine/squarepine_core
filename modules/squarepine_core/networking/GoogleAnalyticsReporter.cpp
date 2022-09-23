namespace networking
{
    inline String createUserAgentValue (bool mustSimulateWebBrowser = true)
    {
        ignoreUnused (mustSimulateWebBrowser);

        const auto osName = SystemStats::getOperatingSystemName().trim();
        auto agentCPUVendor = SystemStats::getCpuVendor().trim();

        if (agentCPUVendor.isEmpty())
            agentCPUVendor = "Unknown CPU";

        String agentOSName = osName.replaceCharacter ('.', '_')
                                    .replace ("iOS", "iPhone OS");
       #if JUCE_IOS
        agentOSName << " like macOS";
       #endif

        String userAgent;
        userAgent << "Mozilla/5.0 ("
                    << SystemStats::getDeviceDescription().trim() << ";"
                    << agentCPUVendor << " "
                    << agentOSName << ";"
                    << SystemStats::getDisplayLanguage() << ")";

        return userAgent;
    }

    inline String createUserAgentHeader (bool mustSimulateWebBrowser = true)
    {
        return "User-Agent: " + createUserAgentValue (mustSimulateWebBrowser) + newLine;
    }
}

#if SQUAREPINE_USE_GOOGLE_ANALYTICS

class GoogleAnalyticsReporter::Sender
{
public:
    Sender (const URL& add, const String& ua, int ms, const StringArray& p) :
        address (add),
        userAgent (ua),
        timeoutMs (ms),
        postData (p)
    {
    }

    /** @note Only for logging purposes. */
    String toString() const
    {
        String s;
        s.preallocateBytes (64);

        s
            << createURL().toString (true) << newLine
            << "User-Agent: " << userAgent << newLine
            << postData.joinIntoString (", ") << newLine;

        return s;
    }

    bool createAndConnect (WebInputStream::Listener* listener = nullptr) const
    {
        auto stream = create();
        return stream != nullptr
            && stream->withExtraHeaders ("User-Agent: " + userAgent)
                      .withConnectionTimeout (timeoutMs)
                      .connect (listener);
    }

private:
    const URL address;
    const String userAgent;
    const int timeoutMs;
    const StringArray postData;

    URL createURL() const { return URL (address).withPOSTData (postData.joinIntoString ("&")); }

    std::unique_ptr<WebInputStream> create() const
    {
        return std::make_unique<WebInputStream> (createURL(), true);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sender)
};

//==============================================================================
class GoogleAnalyticsReporter::ReportPool final : public DeletedAtShutdown
{
public:
    ReportPool()                        { pool.setThreadPriorities (3); }
    ~ReportPool() override              { clearSingletonInstance(); }
    void queue (Sender* senderToUse)    { pool.addJob (new ReportJob (senderToUse), true); }

    JUCE_DECLARE_SINGLETON (ReportPool, true)

private:
    ThreadPool pool;

    class ReportJob final : public ThreadPoolJob
    {
    public:
        ReportJob (Sender* senderToUse) :
            ThreadPoolJob (Uuid().toString()),
            sender (senderToUse)
        {
        }

        JobStatus runJob() override
        {
            sender->createAndConnect();
            return jobHasFinished;
        }

    private:
        std::unique_ptr<Sender> sender;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReportJob)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReportPool)
};

JUCE_IMPLEMENT_SINGLETON (GoogleAnalyticsReporter::ReportPool)

//==============================================================================
class GoogleAnalyticsReporter::Message final : public CallbackMessage
{
public:
    Message (Sender* senderToUse) :
        sender (senderToUse)
    {
    }

    void messageCallback() override
    {
        sender->createAndConnect();
        delete this;
    }

private:
    std::unique_ptr<Sender> sender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Message)
};

//==============================================================================
GoogleAnalyticsMetadata::GoogleAnalyticsMetadata (const String& trackingId,
                                                  const String& clientId,
                                                  HitType hitType)
{
    jassert (trackingId.trim().isNotEmpty());
    jassert (trackingId.startsWith ("UA-")); // No idea what kind of tracking ID you have here...
    jassert (clientId.trim().isNotEmpty());

    // Default general requirements here:
    withProtocolVersion ("1");
    withTrackingId (trackingId);
    withClientId (clientId);
    withHitType (hitType);
}

GoogleAnalyticsMetadata& GoogleAnalyticsMetadata::with (const String& key, const String& value, int maxSizeBytes)
{
    if (key.isNotEmpty())
    {
        String v;

        if (maxSizeBytes > 0)
            v = String::fromUTF8 (value.toRawUTF8(), jmin (maxSizeBytes, (int) CharPointer_UTF8::getBytesRequiredFor (value.getCharPointer())));
        else
            v = value;

        data.set (key, v);
    }

    return *this;
}

GoogleAnalyticsMetadata& GoogleAnalyticsMetadata::with (const String& key, int index, const String& value, int maxSizeBytes)
{
    jassert (index >= 0);

    auto k = key.trim();
    if (k.isNotEmpty() && index >= 0)
    {
        k << index;
        return with (k, value, maxSizeBytes);
    }

    return *this;
}

#undef GA_IMPLEMENT_BOOL_METHOD
#undef GA_IMPLEMENT_INTEGER_METHOD
#undef GA_IMPLEMENT_METHOD_DEFAULT
#undef GA_IMPLEMENT_METHOD

#define GA_IMPLEMENT_METHOD(name, keyValue, maxSizeBytes) \
    GoogleAnalyticsMetadata& GoogleAnalyticsMetadata:: JUCE_JOIN_MACRO (with, name) (const String& keyValue) \
    { \
        return with (JUCE_STRINGIFY (keyValue), keyValue, maxSizeBytes); \
    }

#define GA_IMPLEMENT_BOOL_METHOD(name, keyValue) \
    GoogleAnalyticsMetadata& GoogleAnalyticsMetadata:: JUCE_JOIN_MACRO (with, name) (bool keyValue) \
    { \
        return with (JUCE_STRINGIFY (keyValue), keyValue ? "1" : "0", -1); \
    }

#define GA_IMPLEMENT_INTEGER_METHOD(name, keyValue) \
    GoogleAnalyticsMetadata& GoogleAnalyticsMetadata:: JUCE_JOIN_MACRO (with, name) (int keyValue) \
    { \
        return with (JUCE_STRINGIFY (keyValue), String (keyValue), -1); \
    }

#define GA_IMPLEMENT_METHOD_DEFAULT(name, keyValue) \
    GA_IMPLEMENT_METHOD (name, keyValue, -1)

GA_IMPLEMENT_METHOD_DEFAULT (ProtocolVersion, v)
GA_IMPLEMENT_METHOD_DEFAULT (TrackingId, tid)
GA_IMPLEMENT_BOOL_METHOD (AnonymisedIp, aip)
GA_IMPLEMENT_METHOD_DEFAULT (DataSource, ds)
GA_IMPLEMENT_INTEGER_METHOD (QueueTimeMs, qt)
GA_IMPLEMENT_METHOD_DEFAULT (CacheBuster, z)
GA_IMPLEMENT_METHOD_DEFAULT (ClientId, cid)
GA_IMPLEMENT_METHOD_DEFAULT (UserId, uid)
GA_IMPLEMENT_METHOD_DEFAULT (SessionControl, sc)
GA_IMPLEMENT_METHOD_DEFAULT (IpOverride, uip)
GA_IMPLEMENT_METHOD_DEFAULT (UserAgentOverride, ua)
GA_IMPLEMENT_METHOD_DEFAULT (GeographicalOverride, geoid)
GA_IMPLEMENT_METHOD (DocumentReferrer, dr, 2048)
GA_IMPLEMENT_METHOD (CampaignName, cn, 100)
GA_IMPLEMENT_METHOD (CampaignSource, cs, 100)
GA_IMPLEMENT_METHOD (CampaignMedium, cm, 50)
GA_IMPLEMENT_METHOD (CampaignKeyword, ck, 500)
GA_IMPLEMENT_METHOD (CampaignContent, cc, 500)
GA_IMPLEMENT_METHOD (CampaignId, ci, 100)
GA_IMPLEMENT_METHOD_DEFAULT (GoogleAdWordsId, gclid)
GA_IMPLEMENT_METHOD_DEFAULT (GoogleDisplayAdsId, dclid)
GA_IMPLEMENT_METHOD (ScreenResolution, sr, 20)
GA_IMPLEMENT_METHOD (ViewportSize, vp, 20)
GA_IMPLEMENT_METHOD (DocumentEncoding, de, 20)
GA_IMPLEMENT_METHOD (ScreenColours, sd, 20)
GA_IMPLEMENT_METHOD (UserLanguage, ul, 20)
GA_IMPLEMENT_BOOL_METHOD (NonInteractionHit, ni)
GA_IMPLEMENT_METHOD (DocumentLocationUrl, dl, 2048)
GA_IMPLEMENT_METHOD (DocumentHostName, dh, 100)
GA_IMPLEMENT_METHOD (DocumentPath, dp, 2048)
GA_IMPLEMENT_METHOD (DocumentTitle, dt, 1500)
GA_IMPLEMENT_METHOD (ScreenName, cd, 2048)
GA_IMPLEMENT_METHOD_DEFAULT (LinkId, linkid)
GA_IMPLEMENT_METHOD (ApplicationName, an, 100)
GA_IMPLEMENT_METHOD (ApplicationId, aid, 150)
GA_IMPLEMENT_METHOD (ApplicationVersion, av, 100)
GA_IMPLEMENT_METHOD (ApplicationInstallerId, aiid, 150)
GA_IMPLEMENT_METHOD (EventCategory, ec, 150)
GA_IMPLEMENT_METHOD (EventAction, ea, 500)
GA_IMPLEMENT_METHOD (EventLabel, el, 500)
GA_IMPLEMENT_INTEGER_METHOD (EventValue, ev)
GA_IMPLEMENT_METHOD (TransactionId, ti, 500)
GA_IMPLEMENT_METHOD_DEFAULT (TransactionAffiliation, ta)
GA_IMPLEMENT_METHOD_DEFAULT (TransactionRevenue, tr)
GA_IMPLEMENT_METHOD_DEFAULT (TransactionShipping, ts)
GA_IMPLEMENT_METHOD_DEFAULT (TransactionTax, tt)
GA_IMPLEMENT_METHOD (ItemName, in, 500)
GA_IMPLEMENT_METHOD_DEFAULT (ItemPrice, ip)
GA_IMPLEMENT_INTEGER_METHOD (ItemQuantity, iq)
GA_IMPLEMENT_METHOD (ItemCode, ic, 500)
GA_IMPLEMENT_METHOD (ItemCategory, iv, 500)
GA_IMPLEMENT_METHOD_DEFAULT (ProductAction, pa)
GA_IMPLEMENT_METHOD_DEFAULT (Affiliation, ta)
GA_IMPLEMENT_METHOD_DEFAULT (Revenue, tr)
GA_IMPLEMENT_METHOD_DEFAULT (Tax, tt)
GA_IMPLEMENT_METHOD_DEFAULT (Shipping, ts)
GA_IMPLEMENT_METHOD_DEFAULT (CouponCode, tcc)
GA_IMPLEMENT_METHOD_DEFAULT (ProductActionList, pal)
GA_IMPLEMENT_INTEGER_METHOD (CheckoutStep, cos)
GA_IMPLEMENT_METHOD_DEFAULT (CheckoutStepOption, col)
GA_IMPLEMENT_METHOD_DEFAULT (PromotionAction, promoa)
GA_IMPLEMENT_METHOD (CurrencyCode, cu, 10)
GA_IMPLEMENT_METHOD (SocialNetwork, sn, 50)
GA_IMPLEMENT_METHOD (SocialAction, sa, 50)
GA_IMPLEMENT_METHOD (SocialActionTarget, st, 2048)
GA_IMPLEMENT_METHOD (UserTimingCategory, utc, 150)
GA_IMPLEMENT_METHOD (UserTimingVariableName, utv, 500)
GA_IMPLEMENT_INTEGER_METHOD (UserTimingTimeMs, utt)
GA_IMPLEMENT_METHOD (UserTimingLabel, utl, 500)
GA_IMPLEMENT_METHOD_DEFAULT (PageLoadTime, plt)
GA_IMPLEMENT_INTEGER_METHOD (DNSTimeMs, dns)
GA_IMPLEMENT_INTEGER_METHOD (PageDownloadTimeMs, pdt)
GA_IMPLEMENT_INTEGER_METHOD (RedirectResponseTimeMs, rrt)
GA_IMPLEMENT_INTEGER_METHOD (TCPConnectTimeMs, tcp)
GA_IMPLEMENT_INTEGER_METHOD (ServerResponseTimeMs, srt)
GA_IMPLEMENT_INTEGER_METHOD (DOMInteractiveTimeMs, dit)
GA_IMPLEMENT_INTEGER_METHOD (ContentLoadTimeMs, clt)
GA_IMPLEMENT_METHOD (ExceptionDescription, exd, 150)
GA_IMPLEMENT_BOOL_METHOD (IsExceptionFatal, exf)
GA_IMPLEMENT_METHOD (ExperimentId, xid, 40)
GA_IMPLEMENT_METHOD_DEFAULT (ExperimentVariant, xvar)

#undef GA_IMPLEMENT_BOOL_METHOD
#undef GA_IMPLEMENT_INTEGER_METHOD
#undef GA_IMPLEMENT_METHOD_DEFAULT
#undef GA_IMPLEMENT_METHOD

GoogleAnalyticsMetadata& GoogleAnalyticsMetadata::withHitType (HitType hitType)
{
    String value;

    switch (hitType)
    {
        case HitType::pageView:     value = "pageview"; break;
        case HitType::screenView:   value = "screenview"; break;
        case HitType::event:        value = "event"; break;
        case HitType::transaction:  value = "transaction"; break;
        case HitType::item:         value = "item"; break;
        case HitType::social:       value = "social"; break;
        case HitType::exception:    value = "exception"; break;
        case HitType::timing:       value = "timing"; break;

        default:
            jassertfalse; // New or broken hit type?
        break;
    };

    return with ("t", value);
}

GoogleAnalyticsMetadata& GoogleAnalyticsMetadata::withScreenResolution (int w, int h)
{
    String res;
    res << w << "x" << h;
    return withScreenResolution (res);
}

GoogleAnalyticsMetadata& GoogleAnalyticsMetadata::withScreenResolution()
{
    const auto b = Desktop::getInstance().getDisplays().getRectangleList (false).getBounds();
    return withScreenResolution (b.getWidth(), b.getHeight());
}

GoogleAnalyticsMetadata& GoogleAnalyticsMetadata::withViewportSize (int w, int h)
{
    String res;
    res << w << "x" << h;
    return withViewportSize (res);
}

GoogleAnalyticsMetadata& GoogleAnalyticsMetadata::withViewportSize()
{
    if (const auto* display = Desktop::getInstance().getDisplays().getPrimaryDisplay())
        return withViewportSize (display->totalArea.getWidth(), display->totalArea.getHeight());

    return *this;
}

GoogleAnalyticsMetadata& GoogleAnalyticsMetadata::withCustomDimensions (int startIndex, const StringArray& values)
{
    for (const auto& s : values)
        withCustomDimension (startIndex++, s);

    return *this;
}

GoogleAnalyticsMetadata& GoogleAnalyticsMetadata::withCustomDimension (int index, const String& value)
{
    // There is a maximum of 20 custom dimensions (200 for Analytics 360 accounts).
    // The index suffix must be a positive integer greater than 0 (e.g. "dimension3").
    jassert (index > 0 && index <= 200);

    return with ("cd", index, value, 150);
}

String GoogleAnalyticsMetadata::getDefaultClientId()
{
    return SystemStats::getUniqueDeviceID().trim();
}

//==============================================================================
GoogleAnalyticsReporter::GoogleAnalyticsReporter (const URL& endPointAddress) :
    address (endPointAddress)
{
    jassert (address.isWellFormed() && URL::isProbablyAWebsiteURL (endPointAddress.toString (true)));
}

GoogleAnalyticsReporter::~GoogleAnalyticsReporter()
{
    endSession();
}

void GoogleAnalyticsReporter::setConnectionTimeoutMs (int newTimeoutMs)
{
    timeoutMs = jmax (1000, newTimeoutMs);
}

void GoogleAnalyticsReporter::startSession()
{
}

void GoogleAnalyticsReporter::endSession()
{
}

bool GoogleAnalyticsReporter::sendReport (const StringPairArray& parameters, ReportMethod method)
{
    return sendReport (networking::createUserAgentValue(), parameters, method);
}

bool GoogleAnalyticsReporter::sendReport (const String& userAgent, const StringPairArray& parameters, ReportMethod method)
{
    if (userAgent.isEmpty() || parameters.size() < 4)
    {
        // If you hit this, you're missing fundamental data required
        // for Google Analytics to parse your report!
        jassertfalse;
        return false;
    }

    enum
    {
        protocolVersion = 1,
        trackingId = 2,
        clientId = 4,
        hitType = 8,

        allRequiredKeys = protocolVersion | trackingId | clientId | hitType
    };

    int requiredKeysFound = 0;

    StringArray postData;

    for (const auto& key : parameters.getAllKeys())
    {
        const auto k = key.trim();
        if (k.isNotEmpty())
            postData.add (k + "=" + URL::addEscapeChars (parameters[key].trim(), true));

        if (k == "v")           requiredKeysFound |= protocolVersion;
        else if (k == "tid")    requiredKeysFound |= trackingId;
        else if (k == "cid")    requiredKeysFound |= clientId;
        else if (k == "t")      requiredKeysFound |= hitType;
    }

    postData.trim();
    postData.minimiseStorageOverheads();

    if (requiredKeysFound != allRequiredKeys || postData.isEmpty())
    {
        // If you hit this, you're missing fundamental data required
        // for Google Analytics to parse your report!
        jassertfalse;
        return false;
    }

    if (auto sender = std::make_unique<Sender> (address, userAgent, timeoutMs, postData))
    {
       #if SQUAREPINE_LOG_GOOGLE_ANALYTICS
        Logger::writeToLog ("Google Analytics: Sending new event.\n" + sender->toString());
       #endif

       #if ! SQUAREPINE_ONLY_LOG_GOOGLE_ANALYTICS
        switch (method)
        {
            case ReportMethod::synchronous:     return sender->createAndConnect();
            case ReportMethod::asynchronous:    (new Message (sender.release()))->post(); break;
            case ReportMethod::threaded:        ReportPool::getInstance()->queue (sender.release()); break;

            default:
                jassertfalse; //Unknown method!
                return false;
        };
       #else
        ignoreUnused (method);
       #endif
    }

    return true;
}

bool GoogleAnalyticsReporter::sendSystemReport (GoogleAnalyticsReporter& reporter, const String& trackingId,
                                                const String& clientId, const String& eventAction,
                                                const String& screenName, ReportMethod method)
{
    String appType, appName, appVersion;

   #if defined (JucePlugin_Name)
    appType     = "Plugin";
    appName     = JucePlugin_Name;
    appVersion  = JucePlugin_VersionString;
   #else
    if (JUCEApplicationBase::isStandaloneApp())
    {
        appType = "Application";

        if (auto* app = JUCEApplicationBase::getInstance())
        {
            appName = app->getApplicationName().trim();
            appVersion = app->getApplicationVersion().trim();
        }
    }
    else
    {
        appType = "Library";
    }
   #endif

    StringArray customDims;
    customDims.add (SystemStats::getOperatingSystemName());
    customDims.add (SystemStats::getUserLanguage());
    customDims.add (appType);
    customDims.add (appVersion);
    customDims.add (String ("LogicalCPUs: xyz").replace ("xyz", String (SystemStats::getNumCpus())));
    customDims.add (String ("PhysicalCPUs: xyz").replace ("xyz", String (SystemStats::getNumPhysicalCpus())));
    customDims.add (String ("CPUSpeed: xyz MHz").replace ("xyz", String (SystemStats::getCpuSpeedInMegahertz())));
    customDims.add (String ("CPUVendor: xyz").replace ("xyz", SystemStats::getCpuVendor()));
    customDims.add (String ("CPUModel: xyz").replace ("xyz", SystemStats::getCpuModel()));
    customDims.add (String ("Memory: xyz MB").replace ("xyz", String (SystemStats::getMemorySizeInMegabytes())));

   #if JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
    if (! JUCEApplicationBase::isStandaloneApp())
    {
        customDims.add (String ("PluginFormat: xyz").replace ("xyz", String (AudioProcessor::getWrapperTypeDescription (PluginHostType::getPluginLoadedAs()))));
        customDims.add (String ("PluginHost: xyz").replace ("xyz", String (PluginHostType().getHostDescription())));
    }
   #endif

    customDims.trim();
    customDims.minimiseStorageOverheads();

    GoogleAnalyticsMetadata data (trackingId, clientId);
    data = data.withEventCategory ("stats")
               .withEventAction (eventAction)
               .withScreenName (screenName)
               .withApplicationName (appName)
               .withApplicationVersion (appVersion)
               .withScreenResolution()
               .withViewportSize()
               .withCustomDimensions (1, customDims);

    return reporter.sendReport (data, method);
}

#endif //SQUAREPINE_USE_GOOGLE_ANALYTICS
