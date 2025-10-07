#ifndef SQUAREPINE_CORE_H
#define SQUAREPINE_CORE_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_core
    vendor:             SquarePine
    version:            1.7.0
    name:               SquarePine Core
    description:        A solid backbone for every JUCE project.
    website:            https://www.squarepine.io
    license:            GPLv3
    minimumCppStandard: 20
    dependencies:       juce_opengl juce_cryptography juce_audio_utils
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
/** Current SquarePine major version number. */
#define SQUAREPINE_MAJOR_VERSION        1
/** Current SquarePine minor version number. */
#define SQUAREPINE_MINOR_VERSION        7
/** Current SquarePine build version number. */
#define SQUAREPINE_BUILDNUMBER          0
/** Current SquarePine version number as a string. */
#define SQUAREPINE_VERSION_STRING       "1.7.0"
/** Current SquarePine version number as a string. */
#define SQUAREPINE_FULL_VERSION_STRING  "SquarePine v1.7.0"

//==============================================================================
#undef JUCE_CORE_INCLUDE_JNI_HELPERS
#define JUCE_CORE_INCLUDE_JNI_HELPERS 1

#include <juce_opengl/juce_opengl.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include <juce_core/unit_tests/juce_UnitTestCategories.h>

// NB: Must check for JUCE_WINDOWS because we can't use JUCE_MSVC:
//     when you use Visual Studio and ClangCL, JUCE_MSVC is not defined.
#if ! JUCE_WINDOWS
   #if JUCE_DEBUG
    #include <cxxabi.h>
   #endif

    #include <fenv.h>
    #include <unistd.h>
#endif

//==============================================================================
/** Config: SQUAREPINE_COMPILE_UNIT_TESTS

    Enable or disable compiling SquarePine's unit tests into an application.

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
#include "maths/squarepine_STLHelpers.h"
#include "valuetree/squarepine_VariantConverters.h"

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
    #include "misc/squarepine_Macros.h"
    #include "maths/squarepine_Maths.h"
    #include "valuetree/squarepine_ValueTreeHelpers.h"
    #include "misc/squarepine_MainThreadLogger.h"

    #include "behaviours/squarepine_CreationControl.h"
    #include "behaviours/squarepine_DeletionNotifier.h"
    #include "behaviours/squarepine_Identifiable.h"
    #include "behaviours/squarepine_Lockable.h"
    #include "debugging/squarepine_CrashStackTracer.h"
    #include "maths/squarepine_Algebra.h"
    #include "maths/squarepine_Interpolation.h"
    #include "maths/squarepine_Transforms.h"
    #include "maths/squarepine_Trigonometry.h"
    #include "maths/squarepine_Angle.h"
    #include "maths/squarepine_Ellipse.h"
    #include "maths/squarepine_Line.h"
    #include "maths/squarepine_MovingAccumulator.h"
    #include "maths/squarepine_Polynomials.h"
    #include "maths/squarepine_Steps.h"
    #include "maths/squarepine_Vector4D.h"
    #include "maths/squarepine_Pressure.h"
    #include "maths/squarepine_Temperature.h"
    #include "maths/squarepine_WaterPhaseCalculator.h"
    #include "memory/squarepine_Allocator.h"
    #include "misc/squarepine_Amalgamator.h"
    #include "misc/squarepine_ArrayIterationUnroller.h"
    #include "misc/squarepine_BooleanTools.h"
    #include "misc/squarepine_CodeBeautifiers.h"
    #include "misc/squarepine_CommandHelpers.h"
    #include "misc/squarepine_FPUFlags.h"
    #include "misc/squarepine_Threading.h"
    #include "misc/squarepine_Utilities.h"
    #include "networking/squarepine_GoogleAnalyticsReporter.h"
    #include "networking/squarepine_NetworkCache.h"
    #include "networking/squarepine_NetworkConnectivityChecker.h"
    #include "networking/squarepine_OAuth2.h"
    #include "networking/squarepine_Product.h"
    #include "networking/squarepine_User.h"
    #include "networking/squarepine_Utilities.h"
    #include "networking/squarepine_WebServiceUtilities.h"
    #include "networking/squarepine_WooCommerce.h"
    #include "text/squarepine_LanguageCodes.h"
    #include "text/squarepine_CountryCodes.h"
    #include "text/squarepine_LanguageHandler.h"
    #include "text/squarepine_Utilities.h"
    #include "time/squarepine_StopWatch.h"
    #include "unittests/squarepine_SquarePineCoreUnitTestGatherer.h"
    #include "valuetree/squarepine_JSONToValueTree.h"
    #include "valuetree/squarepine_PropertyContainer.h"
}

#endif // SQUAREPINE_CORE_H
