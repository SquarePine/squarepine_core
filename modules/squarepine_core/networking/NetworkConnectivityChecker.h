/**
*/
class NetworkConnectivityChecker final : private Timer
{
public:
    /** */
    NetworkConnectivityChecker();

    /** */
    ~NetworkConnectivityChecker() override;

    //==============================================================================
    /** */
    void start();

    /** */
    void stop();

    enum class NetworkType
    {
        none,   /*!< There is no network connection. */
        wifi,   /*!< The current network connection is wifi. */
        wired,  /*!< The current network connection is wired, such as an ethernet connection. */
        mobile, /*!< The current network connection is mobile, such as a 3G or 4G network. */
        other   /*!< There is a network connection but the exact type cannot be determined. */
    };

    /** */
    NetworkType getLastKnownNetworkType() const { return networkType; }

    /** */
    NetworkType getCurrentNetworkType() const;

    /** @returns true if an internet connection is present, regardless of how it's present. */
    bool isConnectedToInternet() const;

    //==============================================================================
    /** @returns a normalised (0.0 to 1.0) Received Signal Strength Indicator.

        @note This is only applicable to wifi and mobile data connectivity.
    */
    double getRSSI();

    //==============================================================================
    /** */
    class NetworkListener
    {
    public:
        virtual ~NetworkListener() {}

        /** */
        virtual void networkStatusChanged() = 0;
    };

    /** */
    void addListener (NetworkListener* listener);

    /** */
    void removeListener (NetworkListener* listener);

private:
    //==============================================================================
    NetworkType networkType = NetworkType::none;
    ListenerList<NetworkListener> listeners;

    enum { defaultTimerIntervalMs = 2500 };

    void timerCallback() override;
    void notifyListeners();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NetworkConnectivityChecker)
};

