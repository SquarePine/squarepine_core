enum NetworkStatus : NSInteger
{
    NotReachable = 0,
    ReachableViaWiFi,
    ReachableViaWWAN
};

NSString *kReachabilityChangedNotification = @"kNetworkReachabilityChangedNotification";

@interface Reachability : NSObject

/** Use to check the reachability of a given host name. */
+ (instancetype)reachabilityWithHostName:(NSString *)hostName;

/** Use to check the reachability of a given IP address. */
+ (instancetype)reachabilityWithAddress:(const struct sockaddr *)hostAddress;

/** Checks whether the default route is available.
    Should be used by applications that do not connect to a particular host.
*/
+ (instancetype)reachabilityForInternetConnection;

/**  Start listening for reachability notifications on the current run loop. */
- (BOOL)startNotifier;
- (void)stopNotifier;

- (NetworkStatus)currentReachabilityStatus;

/** WWAN may be available, but not active until a connection has been established.
    WiFi may require a connection for VPN on Demand.
*/
- (BOOL)connectionRequired;

@end


static void ReachabilityCallback(SCNetworkReachabilityRef, SCNetworkReachabilityFlags, void* info)
{
    NSCAssert(info != NULL, @"info was NULL in ReachabilityCallback");
    NSCAssert([(__bridge NSObject*) info isKindOfClass: [Reachability class]], @"info was wrong class in ReachabilityCallback");

    //Post a notification to notify the client that the network reachability changed.
    auto* noteObject = (__bridge Reachability *)info;
    [[NSNotificationCenter defaultCenter] postNotificationName: kReachabilityChangedNotification object: noteObject];
}

@implementation Reachability
{
    SCNetworkReachabilityRef _reachabilityRef;
}

+ (instancetype)reachabilityWithHostName:(NSString *)hostName
{
    auto reachability = SCNetworkReachabilityCreateWithName(nullptr, (const char*)[hostName UTF8String]);
    if (reachability != nullptr)
    {
        Reachability* result = [[self alloc] init];

        if (result != nullptr)
            result->_reachabilityRef = reachability;
        else
            CFRelease(reachability);
            
        return result;
    }

    return nullptr;
}

+ (instancetype)reachabilityWithAddress:(const struct sockaddr *)hostAddress
{
    auto reachability = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, hostAddress);

    Reachability* returnValue = nullptr;

    if (reachability != nullptr)
    {
        returnValue = [[self alloc] init];

        if (returnValue != nullptr)
            returnValue->_reachabilityRef = reachability;
        else
            CFRelease(reachability);
    }

    return returnValue;
}

+ (instancetype)reachabilityForInternetConnection
{
    struct sockaddr_in zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin_len = sizeof(zeroAddress);
    zeroAddress.sin_family = AF_INET;

    return [self reachabilityWithAddress: (const struct sockaddr *) &zeroAddress];
}

- (BOOL)startNotifier
{
    SCNetworkReachabilityContext context = {0, (__bridge void *)(self), nullptr, nullptr, nullptr};

    if (SCNetworkReachabilitySetCallback(_reachabilityRef, ReachabilityCallback, &context))
        if (SCNetworkReachabilityScheduleWithRunLoop(_reachabilityRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode))
            return YES;

    return NO;
}

- (void)stopNotifier
{
    if (_reachabilityRef != nullptr)
        SCNetworkReachabilityUnscheduleFromRunLoop(_reachabilityRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
}

- (void)dealloc
{
    [self stopNotifier];
    if (_reachabilityRef != nullptr)
        CFRelease(_reachabilityRef);

    [super dealloc];
}

- (NetworkStatus)networkStatusForFlags:(SCNetworkReachabilityFlags)flags
{
    // The target host is not reachable.
    if ((flags & kSCNetworkReachabilityFlagsReachable) == 0)
        return NotReachable;

    auto returnValue = NotReachable;

    //If the target host is reachable and no connection is required then we'll assume (for now) that you're on Wi-Fi...
    if ((flags & kSCNetworkReachabilityFlagsConnectionRequired) == 0)
        returnValue = ReachableViaWiFi;

    if ((flags & kSCNetworkReachabilityFlagsConnectionOnDemand) != 0
        || (flags & kSCNetworkReachabilityFlagsConnectionOnTraffic) != 0)
    {
        //and the connection is on-demand (or on-traffic) if the calling application is using the CFSocketStream or higher APIs...
        if ((flags & kSCNetworkReachabilityFlagsInterventionRequired) == 0)
            returnValue = ReachableViaWiFi; //and no [user] intervention is needed...
    }

   #if JUCE_IOS
    //... but WWAN connections are OK if the calling application is using the CFNetwork APIs.
    if ((flags & kSCNetworkReachabilityFlagsIsWWAN) == kSCNetworkReachabilityFlagsIsWWAN)
        returnValue = ReachableViaWWAN;
   #endif

    return returnValue;
}

- (BOOL)connectionRequired
{
    NSAssert(_reachabilityRef != NULL, @"connectionRequired called with NULL reachabilityRef");

    SCNetworkReachabilityFlags flags;
    if (SCNetworkReachabilityGetFlags(_reachabilityRef, &flags))
        return (flags & kSCNetworkReachabilityFlagsConnectionRequired) != 0;

    return NO;
}

- (NetworkStatus)currentReachabilityStatus
{
    NSAssert(_reachabilityRef != NULL, @"currentNetworkStatus called with NULL SCNetworkReachabilityRef");

    SCNetworkReachabilityFlags flags;
    if (SCNetworkReachabilityGetFlags(_reachabilityRef, &flags))
        return [self networkStatusForFlags:flags];

    return NotReachable;
}

@end
