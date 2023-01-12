/** A native API network connection checker.

    Create an instance of this class and add listeners to be notified of any
    state changes pertaining to network connectivity.

    @note This is available on all platforms.
    @note All functionality must be called on the main thread.

    @warning On Android, you will probably need to add these permissions to your manifest,
    and check for them every time you want to check for connectivity:
    - android.permission.ACCESS_NETWORK_STATE
    - android.permission.INTERNET
*/
class NetworkConnectivityChecker final : private Timer
{
public:
    /** Constructor. */
    NetworkConnectivityChecker();

    /** Destructor. */
    ~NetworkConnectivityChecker() override;

    //==============================================================================
    /** Starts checking for status changes to network connectivity.
        If anything changes, this will notify its contained listeners.
    */
    void start();

    /** Stops checking for status changes to network connectivity.
        Listeners will not be notified if anything has changed once this is called.
    */
    void stop();

    //==============================================================================
    enum class NetworkType
    {
        none,   /*!< There is no network connection. */
        wifi,   /*!< The current network connection is wifi. */
        wired,  /*!< The current network connection is wired, such as an ethernet connection. */
        mobile, /*!< The current network connection is mobile, such as a 3G or 4G network. */
        other   /*!< There is a network connection but the exact type cannot be determined (coffee machine?). */
    };

    /** @returns the last scanned for and known network type.
        This may be out of date compared to a call to getCurrentNetworkType().
    */
    NetworkType getLastKnownNetworkType() const { return networkType; }

    /** @returns the most up to date network type as provided by the OS.
        This is used to compare against the last known network type so as to
        update all listeners, and therefore itself.
    */
    NetworkType getCurrentNetworkType() const;

    /** @returns true if an internet connection is present, regardless of how it's present. */
    bool isConnectedToInternet() const;

    //==============================================================================
    /** @returns a normalised (0.0 to 1.0) Received Signal Strength Indicator.
  
        @warning The accuracy and validity of this value is platform dependent.
        In fact, it's probably best to not rely on whatever value is returned here.
        If anything this is just to get a sense of the signal strength, which may
        be 0 on most platforms.

        @note This is only applicable to wifi and mobile data connectivity.
    */
    double getRSSI() const;

    //==============================================================================
    /** Inherit from this to find out if any status changes occur with the internet connectivity.
    
        @warning This will not update your listeners on signal strength status changes!
    */
    class NetworkListener
    {
    public:
        /** */
        virtual ~NetworkListener() { }

        /** */
        virtual void networkStatusChanged() = 0;
    };

    /** Adds a listener, if it wasn't already present. */
    void addListener (NetworkListener*);

    /** Removes a listener if it was present. */
    void removeListener (NetworkListener*);

private:
    //==============================================================================
    NetworkType networkType = NetworkType::none;
    ListenerList<NetworkListener> listeners;

    enum { defaultTimerIntervalMs = 2500 };

    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NetworkConnectivityChecker)
};

