#if SQUAREPINE_USE_GOOGLE_ANALYTICS

/** A convenient wrapper around most of the Google Analytics events.

    This class should be following proper conventions as closely possible!

    @see https://developers.google.com/analytics/devguides/collection/protocol/v1/parameters

    @warning The following parameters must be in each payload, otherwise the report won't be registered:
    - Protocol Version: 'v'
    - Tracking Id: 'tid'
    - Client Id: 'cid'
    - Hit Type: 't'

    @see GoogleAnalyticsReporter
*/
class GoogleAnalyticsMetadata final
{
public:
    /** Possible hit types to choose from.

        @see https://developers.google.com/analytics/devguides/collection/protocol/v1/parameters#t
    */
    enum class HitType
    {
        pageView,
        screenView,
        event,
        transaction,
        item,
        social,
        exception,
        timing
    };

    /** Constructor, whereby the 4 default parameters are added:
        - Protocol Version: 'v'. Note that this is always '1'.
        - Tracking Id: 'tid'
        - Client Id: 'cid'
        - Hit Type: 't'

        @param trackingId This is NOT an optional parameter, so be sure this is not empty.
                          It should look something like this: UA-00000000-0.
    */
    GoogleAnalyticsMetadata (const String& trackingId,
                             const String& clientId = getDefaultClientId(),
                             HitType hitType = HitType::event);

    /** */
    GoogleAnalyticsMetadata (const GoogleAnalyticsMetadata&);
    /** */
    GoogleAnalyticsMetadata (GoogleAnalyticsMetadata&&);
    /** */
    ~GoogleAnalyticsMetadata();

    //==============================================================================
    /** */
    GoogleAnalyticsMetadata& operator= (const GoogleAnalyticsMetadata&);
    /** */
    GoogleAnalyticsMetadata& operator= (GoogleAnalyticsMetadata&&);

    //==============================================================================
    /** @returns a default, typical client ID. */
    static String getDefaultClientId();

    //==============================================================================
    /** */
    const StringPairArray& getData() const noexcept { return data; }

    /** */
    operator const StringPairArray&() const noexcept { return data; }

    //==============================================================================
    /** */
    GoogleAnalyticsMetadata& withProtocolVersion (const String& v);
    /** */
    GoogleAnalyticsMetadata& withTrackingId (const String& tid);
    /** */
    GoogleAnalyticsMetadata& withAnonymisedIp (bool aip);
    /** */
    GoogleAnalyticsMetadata& withDataSource (const String& ds);
    /** */
    GoogleAnalyticsMetadata& withQueueTimeMs (int qt);
    /** */
    GoogleAnalyticsMetadata& withCacheBuster (const String& z);
    /** */
    GoogleAnalyticsMetadata& withClientId (const String& cid);
    /** */
    GoogleAnalyticsMetadata& withUserId (const String& uid);
    /** */
    GoogleAnalyticsMetadata& withSessionControl (const String& sc);
    /** */
    GoogleAnalyticsMetadata& withIpOverride (const String& uip);
    /** */
    GoogleAnalyticsMetadata& withUserAgentOverride (const String& ua);
    /** */
    GoogleAnalyticsMetadata& withGeographicalOverride (const String& geoid);
    /** */
    GoogleAnalyticsMetadata& withDocumentReferrer (const String& dr);
    /** */
    GoogleAnalyticsMetadata& withCampaignName (const String& cn);
    /** */
    GoogleAnalyticsMetadata& withCampaignSource (const String& cs);
    /** */
    GoogleAnalyticsMetadata& withCampaignMedium (const String& cm);
    /** */
    GoogleAnalyticsMetadata& withCampaignKeyword (const String& ck);
    /** */
    GoogleAnalyticsMetadata& withCampaignContent (const String& cc);
    /** */
    GoogleAnalyticsMetadata& withCampaignId (const String& ci);
    /** */
    GoogleAnalyticsMetadata& withGoogleAdWordsId (const String& gclid);
    /** */
    GoogleAnalyticsMetadata& withGoogleDisplayAdsId (const String& dclid);

    /** */
    GoogleAnalyticsMetadata& withScreenResolution (const String& sr);
    /** */
    GoogleAnalyticsMetadata& withScreenResolution (int w, int h);
    /** Takes the entire multi-monitor screen resolution area as information. */
    GoogleAnalyticsMetadata& withScreenResolution();

    /** */
    GoogleAnalyticsMetadata& withViewportSize (const String& vp);
    /** */
    GoogleAnalyticsMetadata& withViewportSize (int w, int h);
    /** */
    GoogleAnalyticsMetadata& withViewportSize();
    /** */
    GoogleAnalyticsMetadata& withDocumentEncoding (const String& de);
    /** */
    GoogleAnalyticsMetadata& withScreenColours (const String& sd);
    /** */
    GoogleAnalyticsMetadata& withUserLanguage (const String& ul);
    /** */
    GoogleAnalyticsMetadata& withHitType (HitType hitType);
    /** */
    GoogleAnalyticsMetadata& withNonInteractionHit (bool ni = true);
    /** */
    GoogleAnalyticsMetadata& withDocumentLocationUrl (const String& dl);
    /** */
    GoogleAnalyticsMetadata& withDocumentHostName (const String& dh);
    /** */
    GoogleAnalyticsMetadata& withDocumentPath (const String& dp);
    /** */
    GoogleAnalyticsMetadata& withDocumentTitle (const String& dt);
    /** */
    GoogleAnalyticsMetadata& withScreenName (const String& cd);
    /** */
    GoogleAnalyticsMetadata& withLinkId (const String& linkid);
    /** */
    GoogleAnalyticsMetadata& withApplicationName (const String& an);
    /** */
    GoogleAnalyticsMetadata& withApplicationId (const String& aid);
    /** */
    GoogleAnalyticsMetadata& withApplicationVersion (const String& av);
    /** */
    GoogleAnalyticsMetadata& withApplicationInstallerId (const String& aiid);
    /** */
    GoogleAnalyticsMetadata& withEventCategory (const String& ec);
    /** */
    GoogleAnalyticsMetadata& withEventAction (const String& ea);
    /** */
    GoogleAnalyticsMetadata& withEventLabel (const String& el);
    /** */
    GoogleAnalyticsMetadata& withEventValue (int ev);
    /** */
    GoogleAnalyticsMetadata& withTransactionId (const String& ti);
    /** */
    GoogleAnalyticsMetadata& withTransactionAffiliation (const String& ta);
    /** */
    GoogleAnalyticsMetadata& withTransactionRevenue (const String& tr);
    /** */
    GoogleAnalyticsMetadata& withTransactionShipping (const String& ts);
    /** */
    GoogleAnalyticsMetadata& withTransactionTax (const String& tt);
    /** */
    GoogleAnalyticsMetadata& withItemName (const String& in);
    /** */
    GoogleAnalyticsMetadata& withItemPrice (const String& ip);
    /** */
    GoogleAnalyticsMetadata& withItemQuantity (int iq);
    /** */
    GoogleAnalyticsMetadata& withItemCode (const String& ic );
    /** */
    GoogleAnalyticsMetadata& withItemCategory (const String& iv);
    /** */
    GoogleAnalyticsMetadata& withProductAction (const String& pa);
    /** */
    GoogleAnalyticsMetadata& withAffiliation (const String& ta);
    /** */
    GoogleAnalyticsMetadata& withRevenue (const String& tr);
    /** */
    GoogleAnalyticsMetadata& withTax (const String& tt);
    /** */
    GoogleAnalyticsMetadata& withShipping (const String& ts);
    /** */
    GoogleAnalyticsMetadata& withCouponCode (const String& tcc);
    /** */
    GoogleAnalyticsMetadata& withProductActionList (const String& pal);
    /** */
    GoogleAnalyticsMetadata& withCheckoutStep (int cos);
    /** */
    GoogleAnalyticsMetadata& withCheckoutStepOption (const String& col);
    /** */
    GoogleAnalyticsMetadata& withPromotionAction (const String& promoa);
    /** */
    GoogleAnalyticsMetadata& withCurrencyCode (const String& cu);
    /** */
    GoogleAnalyticsMetadata& withSocialNetwork (const String& sn);
    /** */
    GoogleAnalyticsMetadata& withSocialAction (const String& sa);
    /** */
    GoogleAnalyticsMetadata& withSocialActionTarget (const String& st);
    /** */
    GoogleAnalyticsMetadata& withUserTimingCategory (const String& utc);
    /** */
    GoogleAnalyticsMetadata& withUserTimingVariableName (const String& utv);
    /** */
    GoogleAnalyticsMetadata& withUserTimingTimeMs (int utt);
    /** */
    GoogleAnalyticsMetadata& withUserTimingLabel (const String& utl);
    /** */
    GoogleAnalyticsMetadata& withPageLoadTime (const String& plt);
    /** */
    GoogleAnalyticsMetadata& withDNSTimeMs (int dns);
    /** */
    GoogleAnalyticsMetadata& withPageDownloadTimeMs (int pdt);
    /** */
    GoogleAnalyticsMetadata& withRedirectResponseTimeMs (int rrt);
    /** */
    GoogleAnalyticsMetadata& withTCPConnectTimeMs (int tcp);
    /** */
    GoogleAnalyticsMetadata& withServerResponseTimeMs (int srt);
    /** */
    GoogleAnalyticsMetadata& withDOMInteractiveTimeMs (int dit);
    /** */
    GoogleAnalyticsMetadata& withContentLoadTimeMs (int clt);
    /** */
    GoogleAnalyticsMetadata& withExceptionDescription (const String& exd);
    /** */
    GoogleAnalyticsMetadata& withIsExceptionFatal (bool exf = true);
    /** */
    GoogleAnalyticsMetadata& withCustomDimension (int index, const String& value);
    /** */
    GoogleAnalyticsMetadata& withCustomDimensions (int startIndex, const StringArray& values);

    /** @see https://developers.google.com/analytics/devguides/collection/analyticsjs/custom-dims-mets */
    template<typename IntOrFloatType>
    GoogleAnalyticsMetadata& withCustomMetric (int index, IntOrFloatType value)
    {
        jassert (index > 0 && index <= 200); //There is a maximum of 20 custom metrics (200 for Analytics 360 accounts).
                                             //The index suffix must be a positive integer greater than 0 (e.g. metric5).

        return with ("cm", index, String (value));
    }

    /** @todo */
    GoogleAnalyticsMetadata& withExperimentId (const String& value);
    /** @todo */
    GoogleAnalyticsMetadata& withExperimentVariant (const String& value);

    /** */
    GoogleAnalyticsMetadata& with (const String& key, const String& value, int maxSizeBytes = -1);
    /** */
    GoogleAnalyticsMetadata& with (const String& key, int index, const String& value, int maxSizeBytes = -1);

private:
    //==============================================================================
    StringPairArray data;

    //==============================================================================
    JUCE_LEAK_DETECTOR (GoogleAnalyticsMetadata)
};

//==============================================================================
/**

    @see https://developers.google.com/analytics/devguides/collection/protocol/v1/reference#endpoint

    @see GoogleAnalyticsMetadata
*/
class GoogleAnalyticsReporter final
{
public:
    /** Constructor.

        @param address Typically this is the default Google Analytics end-point to POST to,
                       but Google doesn't change this so a default parameter has been set.
    */
    GoogleAnalyticsReporter (const URL& endPointAddress = URL ("https://www.google-analytics.com/collect"));

    //==============================================================================
    enum class ReportType
    {
        /** Sends off the report synchronously.

            You should be careful with this, as this will block the calling thread.
            And if there isn't an internet connection, this will be blocking for the amount of
            time the connection timeout is set to!
        */
        synchronousReport,

        /** Sends off the report asynchronously.

            This will use the MessageManager to queue up a network request.
        */
        asynchronousReport,

        /** Sends off the report asynchronously, on a different thread.

            This will use an internal thread to queue up a network request.
            Note that queue many requests at once may bring up the CPU load quickly,
            especially if the requests are having to wait for an internet connection.
        */
        threadedReport
    };

    /** Sends a customised report.

        @param parameters The group of parameters Google Analytics can understand.
                          For an easier means of filling these out, use GoogleAnalyticsMetadata.
    */
    bool sendReport (const StringPairArray& parameters, ReportType type = ReportType::threadedReport);

    /** Sends a customised report.

        @param userAgent    The user agent to use. If you don't know what this should be,
                            use the other version of this method.
        @param parameters   The group of parameters Google Analytics can understand.
                            For an easier means of filling these out, use GoogleAnalyticsMetadata.
    */
    bool sendReport (const String& userAgent, const StringPairArray& parameters, ReportType type = ReportType::threadedReport);

    //==============================================================================
    /** Change the connection timeout to something else, in milliseconds.

        By default this is 3 seconds, but this is here in case you want to change it.

        This value should be relatively forgiving, so be sure that you account
        for slower internet connections!
    */
    void setConnectionTimeoutMs (int newTimeoutMs);

    /** @returns the current timeout in milliseconds. */
    int getConnectionTimeoutMs() const noexcept { return timeoutMs; }

    //==============================================================================
    /** Sends a standardised and detailed system report.

        This doesn't respect the custom dimension indexes so be aware that this
        will definitely mess with your reports and previously logged custom dimensions!

        @param trackingId   This is the 'tid' that will be set in the parameters.
                            This is NOT an optional parameter, so be sure this is not empty
                            and looks something like: UA-00000000-0.
        @param clientId     This is the 'ea' that will be set in the parameters.
                            @see getDefaultClientId()
        @param eventAction  This is the 'ea' that will be set in the parameters.
                            @see GoogleAnalyticsMetadata::withEventAction.
        @param screenName   This is the 'cd' that will be set in the parameters.
                            @see GoogleAnalyticsMetadata::withScreenName.
    */
    static bool sendSystemReport (const String& trackingId,
                                  const String& clientId = GoogleAnalyticsMetadata::getDefaultClientId(),
                                  const String& eventAction = "AppStart",
                                  const String& screenName = "Main",
                                  ReportType type = ReportType::threadedReport);

private:
    //==============================================================================
    class Sender;
    class Message;
    class ReportPool;

    const URL address;
    int timeoutMs = 3000;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GoogleAnalyticsReporter)
};

#endif //SQUAREPINE_USE_GOOGLE_ANALYTICS
