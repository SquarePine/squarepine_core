namespace
{
    inline sp::NetworkConnectivityChecker::NetworkType getCurrentSystemNetworkType()
    {
        Reachability* reachability = [Reachability reachabilityForInternetConnection];
        NetworkStatus status = [reachability currentReachabilityStatus];

        if (status == NotReachable)             return sp::NetworkConnectivityChecker::NetworkType::none;
        else if (status == ReachableViaWiFi)    return sp::NetworkConnectivityChecker::NetworkType::wifi;
        else if (status == ReachableViaWWAN)    return sp::NetworkConnectivityChecker::NetworkType::mobile;

        return sp::NetworkConnectivityChecker::NetworkType::wired;
    }

    double getCurrentSystemRSSI()
    {
        // According to [https://forums.developer.apple.com/thread/67932]
        // Apple has has no interest on making RSSI available thru API
        return 1.0;
    }
}
