#undef JUCE_CORE_INCLUDE_NATIVE_HEADERS
#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1

#undef JUCE_CORE_INCLUDE_OBJC_HELPERS
#define JUCE_CORE_INCLUDE_OBJC_HELPERS 1

#include "squarepine_core.h"

#if JUCE_MAC || JUCE_IOS
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>

    #import <CoreFoundation/CoreFoundation.h>
    #import <SystemConfiguration/SystemConfiguration.h>
    #include <SystemConfiguration/SCNetwork.h>
    #include "networking/Reachability.m"
    #include "networking/NetworkConnectivityCheckerApple.mm"
#endif

#include "squarepine_core.cpp"
