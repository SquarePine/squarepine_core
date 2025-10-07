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

    #include "debugging/squarepine_CrashStackTracer.cpp"
    #include "misc/squarepine_ArrayIterationUnroller.cpp"
    #include "misc/squarepine_CodeBeautifiers.cpp"
    #include "misc/squarepine_CommandHelpers.cpp"
    #include "misc/squarepine_FPUFlags.cpp"
    #include "misc/squarepine_MainThreadLogger.cpp"
    #include "networking/squarepine_GoogleAnalyticsReporter.cpp"
    #include "networking/squarepine_NetworkCache.cpp"

    //==============================================================================
    double getCurrentSystemRSSI();
    NetworkConnectivityChecker::NetworkType getCurrentSystemNetworkType();

    #include "networking/squarepine_NetworkConnectivityCheckerPosix.cpp"
    #include "networking/squarepine_NetworkConnectivityCheckerWindows.cpp"
    #include "networking/squarepine_NetworkConnectivityChecker.cpp"

    //==============================================================================
    #include "networking/squarepine_OAuth2.cpp"
    #include "networking/squarepine_Product.cpp"
    #include "networking/squarepine_User.cpp"
    #include "networking/squarepine_WebServiceUtilities.cpp"
    #include "networking/squarepine_WooCommerce.cpp"
    #include "text/squarepine_LanguageCodes.cpp"
    #include "text/squarepine_CountryCodes.cpp"
    #include "text/squarepine_LanguageHandler.cpp"
    #include "text/squarepine_Utilities.cpp"
    #include "valuetree/squarepine_JSONToValueTree.cpp"

    #include "unittests/squarepine_AllocatorUnitTests.cpp"
    #include "unittests/squarepine_AngleUnitTests.cpp"
    #include "unittests/squarepine_MathsUnitTests.cpp"
    #include "unittests/squarepine_SquarePineCoreUnitTestGatherer.cpp"
}
