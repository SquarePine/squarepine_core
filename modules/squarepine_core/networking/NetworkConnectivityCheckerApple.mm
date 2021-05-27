namespace sp
{
    using namespace juce;

    NetworkConnectivityChecker::NetworkType getCurrentSystemNetworkType()
    {
        Reachability* reachability = [Reachability reachabilityForInternetConnection];
        NetworkStatus status = [reachability currentReachabilityStatus];

        if (status == NotReachable)             return NetworkConnectivityChecker::NetworkType::none;
        else if (status == ReachableViaWiFi)    return NetworkConnectivityChecker::NetworkType::wifi;
        else if (status == ReachableViaWWAN)    return NetworkConnectivityChecker::NetworkType::mobile;

        return NetworkConnectivityChecker::NetworkType::wired;
    }

    double getCurrentSystemRSSI()
    {
        // According to [https://forums.developer.apple.com/thread/67932]
        // Apple has has no interest on making RSSI available thru API
        return 1.0;
    }
}
