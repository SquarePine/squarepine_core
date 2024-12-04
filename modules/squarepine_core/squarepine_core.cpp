#if ! JUCE_MAC && ! JUCE_IOS
    #undef JUCE_CORE_INCLUDE_NATIVE_HEADERS
    #define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1
#endif

#define JUCE_CORE_INCLUDE_JNI_HELPERS 1

#include "squarepine_core.h"

#if JUCE_WINDOWS
    #include <netlistmgr.h>
    #include <Wlanapi.h>
    #pragma comment (lib, "iphlpapi.lib")
    #pragma comment (lib, "wlanapi.lib")
#elif JUCE_ANDROID
    #include <sys/system_properties.h>
#endif

//==============================================================================
// Just in case some idiotic library or system header is up to no good...
#undef GET
#undef HEAD
#undef POST
#undef PUT
#undef DELETE
#undef CONNECT
#undef OPTIONS
#undef TRACE

//==============================================================================
namespace sp
{
    String getAndroidReleaseVersion()
    {
       #if JUCE_ANDROID
        char osVersion[PROP_VALUE_MAX + 1] = { 0 };
        __system_property_get ("ro.build.version.release", osVersion);
        return String (osVersion).trim();
       #else
        return {};
       #endif
    }

    #include "debugging/CrashStackTracer.cpp"
    #include "misc/ArrayIterationUnroller.cpp"
    #include "misc/CodeBeautifiers.cpp"
    #include "misc/CommandHelpers.cpp"
    #include "misc/FPUFlags.cpp"
    #include "misc/MainThreadLogger.cpp"
    #include "networking/GoogleAnalyticsReporter.cpp"
    #include "networking/NetworkCache.cpp"

    //==============================================================================
    double getCurrentSystemRSSI();
    NetworkConnectivityChecker::NetworkType getCurrentSystemNetworkType();

    #include "networking/NetworkConnectivityCheckerPosix.cpp"
    #include "networking/NetworkConnectivityCheckerWindows.cpp"
    #include "networking/NetworkConnectivityChecker.cpp"

    //==============================================================================
   #if JUCE_ANDROID
    #include "native/squarepine_RuntimePermissions_Android.cpp"
   #else
    #include "native/squarepine_RuntimePermissions_generic.cpp"
   #endif

    #include "networking/OAuth2.cpp"
    #include "networking/Product.cpp"
    #include "networking/User.cpp"
    #include "networking/WebServiceUtilities.cpp"
    #include "networking/WooCommerce.cpp"
    #include "text/LanguageCodes.cpp"
    #include "text/CountryCodes.cpp"
    #include "text/LanguageHandler.cpp"
    #include "text/Utilities.cpp"
    #include "valuetree/JSONToValueTree.cpp"

    #include "unittests/AllocatorUnitTests.cpp"
    #include "unittests/AngleUnitTests.cpp"
    #include "unittests/MathsUnitTests.cpp"
    #include "unittests/SquarePineCoreUnitTestGatherer.cpp"
}
