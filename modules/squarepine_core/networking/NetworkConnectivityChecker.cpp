//==============================================================================
NetworkConnectivityChecker::NetworkConnectivityChecker()                { networkType = getCurrentNetworkType(); start(); }
NetworkConnectivityChecker::~NetworkConnectivityChecker()               { }
void NetworkConnectivityChecker::start()                                { startTimer (NetworkConnectivityChecker::defaultTimerIntervalMs); }
void NetworkConnectivityChecker::stop()                                 { stopTimer(); }
bool NetworkConnectivityChecker::isConnectedToInternet() const          { return getCurrentNetworkType() != NetworkType::none; }
void NetworkConnectivityChecker::addListener (NetworkListener* l)       { listeners.add (l); }
void NetworkConnectivityChecker::removeListener (NetworkListener* l)    { listeners.remove (l); }

NetworkConnectivityChecker::NetworkType NetworkConnectivityChecker::getCurrentNetworkType() const
{
    return getCurrentSystemNetworkType();
}

void NetworkConnectivityChecker::timerCallback()
{
    const auto currentNetworkType = getCurrentNetworkType();

    /** NB: This weird looking condition is just checking if the last known state
            was connected and now it's disconnected, or vice-versa.
            We don't care about the type of network in this case.
            ie: "connected" was 1 now 0, or was 0 now 1
    */
    if ((networkType != NetworkType::none) != (currentNetworkType != NetworkType::none))
    {
        networkType = currentNetworkType;

        listeners.call ([&] (NetworkConnectivityChecker::NetworkListener& l) { l.networkStatusChanged(); });
    }
}

double NetworkConnectivityChecker::getRSSI() const
{
    const auto currentNetworkType = getCurrentNetworkType();
    if (currentNetworkType == NetworkType::none)
        return 0.0;

    if (currentNetworkType != NetworkType::wifi
        && currentNetworkType != NetworkType::mobile)
        return 1.0;

    return getCurrentSystemRSSI();
}
