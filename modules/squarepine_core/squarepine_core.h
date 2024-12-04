#ifndef SQUAREPINE_CORE_H
#define SQUAREPINE_CORE_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_core
    vendor:             SquarePine
    version:            2.0.0
    name:               SquarePine Core
    description:        A decent backbone for any project.
    website:            https://www.squarepine.io
    license:            Beerware
    minimumCppStandard: 20
    dependencies:       juce_audio_utils juce_cryptography juce_opengl
    OSXFrameworks:      SystemConfiguration
    iOSFrameworks:      SystemConfiguration

    END_JUCE_MODULE_DECLARATION
*/

//==============================================================================
// NB: Both are needed in order to satisfy old and crusty GCC (mostly for CI).
#include <climits>
#include <limits>

#include <bitset>
#include <forward_list>
#include <list>
#include <optional>
#include <random>
#include <regex>
#include <stack>
#include <type_traits>

//==============================================================================
#undef JUCE_CORE_INCLUDE_JNI_HELPERS
#define JUCE_CORE_INCLUDE_JNI_HELPERS 1

#include <juce_opengl/juce_opengl.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_cryptography/juce_cryptography.h>

#include <juce_core/unit_tests/juce_UnitTestCategories.h>

#if JUCE_MINGW
    #error "SquarePine: JUCE has officially removed support for MinGW and I don't want to support it myself, either."
#endif

#if ! JUCE_WINDOWS
   #if JUCE_DEBUG
    #include <cxxabi.h>
   #endif

    #include <fenv.h>
    #include <unistd.h>
#endif

//==============================================================================
/** Config: SQUAREPINE_COMPILE_UNIT_TESTS

    Enable or disable compiling unit tests into an application.

    By default this is off.
*/
#ifndef SQUAREPINE_COMPILE_UNIT_TESTS
    #define SQUAREPINE_COMPILE_UNIT_TESTS 0
#endif

/** Config: SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR

    Enable this to compare performance between linear iteration
    and loop unrolling with the ArrayIterationUnroller.
*/
#ifndef SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR
    #define SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR 0
#endif

/** Config: SQUAREPINE_ARRAY_ITERATION_UNROLLER_CHECK_BIG_NUMS
*/
#ifndef SQUAREPINE_ARRAY_ITERATION_UNROLLER_CHECK_BIG_NUMS
    #define SQUAREPINE_ARRAY_ITERATION_UNROLLER_CHECK_BIG_NUMS 0
#endif

/** Config: SQUAREPINE_LOG_NETWORK_CALLS

    Enable or disable logging networking calls.

    By default this is on for debugging and disabled in release builds.
*/
#ifndef SQUAREPINE_LOG_NETWORK_CALLS
    #define SQUAREPINE_LOG_NETWORK_CALLS JUCE_DEBUG
#endif

/** Config: SQUAREPINE_AUTOCONFIG_MAIN_THREAD_LOG_FILTERS

    Enable this to automatically configure the MainThreadLogger's
    filter to "debug" in Debug and to "warnings" in Release.

    If you disable this, you will need to manually change the filter
    to suit your needs because it will be set to debug by default.

    @see MainThreadLogger
*/
#ifndef SQUAREPINE_AUTOCONFIG_MAIN_THREAD_LOG_FILTERS
    #define SQUAREPINE_AUTOCONFIG_MAIN_THREAD_LOG_FILTERS 1
#endif

/** Config: SQUAREPINE_AUTOLOG_FUNCTION_AND_LINE

    Enable this to automatically configure the various
    logging functions to track the calling function and line number.

    By default, this is available in debug builds only.
    This is just in case you don't want to leak secret details
    in a Release build!

    Only available with C++20 or later.

    @see MainThreadLogger
*/
#ifndef SQUAREPINE_AUTOLOG_FUNCTION_AND_LINE
    #define SQUAREPINE_AUTOLOG_FUNCTION_AND_LINE JUCE_DEBUG
#endif

/** Config: SQUAREPINE_USE_GOOGLE_ANALYTICS

    Enable or disable using Google Analytics based on
    our own fully featured implementation.

    By default this is on.
*/
#ifndef SQUAREPINE_USE_GOOGLE_ANALYTICS
    #define SQUAREPINE_USE_GOOGLE_ANALYTICS 1
#endif

/** Config: SQUAREPINE_LOG_GOOGLE_ANALYTICS

    Enables or disables the logging of Google Analytics events.
    Use this to troubleshoot events and such.

    By default this is off.
*/
#ifndef SQUAREPINE_LOG_GOOGLE_ANALYTICS
    #define SQUAREPINE_LOG_GOOGLE_ANALYTICS 0
#endif

/** Config: SQUAREPINE_ONLY_LOG_GOOGLE_ANALYTICS

    Enabling this forces Google Analytics to strictly
    log the messages to the usual Logger::writeToLog;
    no messages will be sent out.
*/
#ifndef SQUAREPINE_ONLY_LOG_GOOGLE_ANALYTICS
    #define SQUAREPINE_ONLY_LOG_GOOGLE_ANALYTICS 0
#endif

#if SQUAREPINE_ONLY_LOG_GOOGLE_ANALYTICS
    #undef SQUAREPINE_LOG_GOOGLE_ANALYTICS
    #define SQUAREPINE_LOG_GOOGLE_ANALYTICS 1
#endif

// This bit allows logging the host type in Google Analytics in the system logging:
#if SQUAREPINE_USE_GOOGLE_ANALYTICS && JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
    #include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#endif

//==============================================================================
#if JUCE_MSVC
    // JUCE's test for C++20 availability in MSVC is wrong. Till then...
   #undef JUCE_CXX20_IS_AVAILABLE
   #ifndef JUCE_CXX20_IS_AVAILABLE
    #define JUCE_CXX20_IS_AVAILABLE (_HAS_CXX20)
   #endif

   #ifndef JUCE_CXX23_IS_AVAILABLE
    #define JUCE_CXX23_IS_AVAILABLE (_HAS_CXX23)
   #endif
#else
   #ifndef JUCE_CXX23_IS_AVAILABLE
    #define JUCE_CXX23_IS_AVAILABLE (__cplusplus >= 202300L)
   #endif
#endif

#if JUCE_CXX20_IS_AVAILABLE
    #include <bit>

    // Android doesn't have this because... Android.
   #if ! JUCE_ANDROID
    #include <source_location>
   #endif
#endif

#if ! JUCE_CXX20_IS_AVAILABLE || JUCE_ANDROID
    #undef SQUAREPINE_AUTOLOG_FUNCTION_AND_LINE
#endif

//==============================================================================
#if JUCE_CLANG || JUCE_GCC
    #pragma clang diagnostic warning "-Wall"
    #pragma clang diagnostic warning "-Wconstant-conversion"
    #pragma clang diagnostic warning "-Wconversion"
    #pragma clang diagnostic warning "-Wextra-semi"
    #pragma clang diagnostic warning "-Wint-conversion"
    #pragma clang diagnostic warning "-Wnewline-eof"
    #pragma clang diagnostic warning "-Wreorder"
    #pragma clang diagnostic warning "-Wshadow"
    #pragma clang diagnostic warning "-Wshorten-64-to-32"
    #pragma clang diagnostic warning "-Wsign-compare"
    #pragma clang diagnostic warning "-Wsign-conversion"
    #pragma clang diagnostic warning "-Wstrict-aliasing"
    #pragma clang diagnostic warning "-Wuninitialized"
    #pragma clang diagnostic warning "-Wunused-parameter"
    #pragma clang diagnostic warning "-Wnon-virtual-dtor"
    #pragma clang diagnostic warning "-Woverloaded-virtual"
    #pragma clang diagnostic warning "-Wunused-variable"

    #pragma clang diagnostic ignored "-Wswitch-enum" // Fairly useless warning...
#elif JUCE_MSVC
    #pragma warning (default: 4242 4254 4264 4265 4287 4296 4302 4342 4350)
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
#include "valuetree/VariantConverters.h"

//==============================================================================
namespace sp
{
    using namespace juce;

    //==============================================================================
    /** @returns Android's release version, which should look
        something like "Android 13".

        This will be empty if something goes wrong; who knows if you can
        call this outside of the main thread, who knows what manufacturers
        will do, etc...
    */
    String getAndroidReleaseVersion();

    //==============================================================================
    #include "misc/Macros.h"
    #include "maths/Maths.h"
    #include "valuetree/ValueTreeHelpers.h"
    #include "misc/MainThreadLogger.h"

    #include "behaviours/CreationControl.h"
    #include "behaviours/DeletionNotifier.h"
    #include "behaviours/Identifiable.h"
    #include "behaviours/Lockable.h"
    #include "debugging/CrashStackTracer.h"
    #include "maths/Algebra.h"
    #include "maths/Interpolation.h"
    #include "maths/Transforms.h"
    #include "maths/Trigonometry.h"
    #include "maths/Angle.h"
    #include "maths/Curves.h"
    #include "maths/Easing.h"
    #include "maths/Ellipse.h"
    #include "maths/Line.h"
    #include "maths/MovingAccumulator.h"
    #include "maths/Polynomials.h"
    #include "maths/Spline.h"
    #include "maths/Steps.h"
    #include "maths/Vector4D.h"
    #include "memory/Allocator.h"
    #include "misc/Amalgamator.h"
    #include "misc/ArrayIterationUnroller.h"
    #include "misc/BooleanTools.h"
    #include "misc/CodeBeautifiers.h"
    #include "misc/CommandHelpers.h"
    #include "misc/FPUFlags.h"
    #include "misc/Threading.h"
    #include "misc/Utilities.h"
    #include "native/squarepine_RuntimePermissions_generic.h"
    #include "networking/GoogleAnalyticsReporter.h"
    #include "networking/NetworkCache.h"
    #include "networking/NetworkConnectivityChecker.h"
    #include "networking/OAuth2.h"
    #include "networking/Product.h"
    #include "networking/User.h"
    #include "networking/Utilities.h"
    #include "networking/WebServiceUtilities.h"
    #include "networking/WooCommerce.h"
    #include "text/LanguageCodes.h"
    #include "text/CountryCodes.h"
    #include "text/LanguageHandler.h"
    #include "text/Utilities.h"
    #include "time/StopWatch.h"
    #include "unittests/SquarePineCoreUnitTestGatherer.h"
    #include "valuetree/JSONToValueTree.h"
    #include "valuetree/PropertyContainer.h"
}

#endif // SQUAREPINE_CORE_H
