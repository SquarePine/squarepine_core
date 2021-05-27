#ifndef SQUAREPINE_CORE_H
#define SQUAREPINE_CORE_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_core
    vendor:             SquarePine
    version:            0.0.1
    name:               SquarePine Core
    description:        A decent backbone for any project.
    website:            https://www.squarepine.io
    license:            Proprietary
    minimumCppStandard: 14
    OSXFrameworks:      SystemConfiguration
    iOSFrameworks:      SystemConfiguration
    dependencies:       juce_audio_utils juce_cryptography juce_opengl

    END_JUCE_MODULE_DECLARATION
*/

//==============================================================================
// NB: Both are needed in order to satisfy old and crusty GCC (mostly for CI).
#include <climits>
#include <limits>

#include <forward_list>
#include <list>
#include <random>
#include <regex>

//==============================================================================
#include <juce_opengl/juce_opengl.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_cryptography/juce_cryptography.h>

#include <juce_core/unit_tests/juce_UnitTestCategories.h>

#if ! JUCE_WINDOWS
   #if JUCE_DEBUG
    #include <cxxabi.h>
   #endif

    #include <fenv.h>
    #include <unistd.h>
#endif

#if JUCE_MINGW
    #error "First of all, you're on Windows, not Linux." \
           "Secondly, MinGW is too outdated and crappy." \
           "Seriously, just use Visual Studio Community - it's literally free."
#endif

//==============================================================================
/** Config: SQUAREPINE_COMPILE_UNIT_TESTS

    Enable or disable compiling unit tests into an application.

    By default, this is off.
*/
#ifndef SQUAREPINE_COMPILE_UNIT_TESTS
    #define SQUAREPINE_COMPILE_UNIT_TESTS 0
#endif

/** Config: SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR

    Enable this to compare performance between linear iteration and loop unrolling with the ArrayIterationUnroller.
*/
#ifndef SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR
    #define SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR 0
#endif

/** Config: SQUAREPINE_ARRAY_ITERATION_UNROLLER_CHECK_BIG_NUMS
*/
#ifndef SQUAREPINE_ARRAY_ITERATION_UNROLLER_CHECK_BIG_NUMS
    #define SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR 0
#endif

/** Config: SQUAREPINE_LOG_NETWORK_CALLS

    Enable or disable logging networking calls.

    By default, this is on for debugging, and disabled in release builds.
*/
#ifndef SQUAREPINE_LOG_NETWORK_CALLS
    #define SQUAREPINE_LOG_NETWORK_CALLS JUCE_DEBUG
#endif

/** Config: SQUAREPINE_USE_GOOGLE_ANALYTICS

    Enable or disable using Google Analytics based on our own fully featured implementation.

    By default, this is on.
*/
#ifndef SQUAREPINE_USE_GOOGLE_ANALYTICS
    #define SQUAREPINE_USE_GOOGLE_ANALYTICS 1
#endif

// This bit allows logging the host type in Google Analytics in the system logging:
#if SQUAREPINE_USE_GOOGLE_ANALYTICS && JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
    #include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#endif

//==============================================================================
#if JUCE_MSVC && (__cplusplus == 199711L)
   #if ! defined (_MSVC_LANG)
    #error "We need to find another method to determine the C++ version at compile time!"
   #endif

   #if defined (_MSVC_LANG)
    #ifndef JUCE_CXX20_IS_AVAILABLE
     #define JUCE_CXX20_IS_AVAILABLE (_MSVC_LANG >= 202000L)
    #endif
   #endif
#else
   #ifndef JUCE_CXX20_IS_AVAILABLE
    #define JUCE_CXX20_IS_AVAILABLE (__cplusplus >= 202000L)
   #endif
#endif

//==============================================================================
#if JUCE_CXX20_IS_AVAILABLE
    #include <bit>
#endif

//==============================================================================
#undef sp_nodiscard
#undef SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE

#if JUCE_CXX17_IS_AVAILABLE
    #include <optional>

    #define SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE(Type) \
        std::optional<Type>

    #define sp_nodiscard [[nodiscard]]
#else
    #define SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE(Type) \
        Type

    #define sp_nodiscard

    namespace std
    {
        template<class T>
        sp_nodiscard constexpr const T& clamp (const T& v, const T& lo, const T& hi) noexcept
        {
            return (v < lo) ? lo : (hi < v) ? hi : v;
        }

        template<class T, class Predicate>
        sp_nodiscard constexpr const T& clamp (const T& v, const T& lo, const T& hi, Predicate predicate) noexcept
        {
            return predicate (v, lo) ? lo : predicate (hi, v) ? hi : v;
        }
    }

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
    #pragma warning (default: 4242 4254 4264 4265 4287 4296 4302 4342 4350 4355)
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
    #include "misc/Macros.h"
    #include "maths/Maths.h"
    #include "valuetree/ValueTreeHelpers.h"

    #include "behaviours/CreationControl.h"
    #include "behaviours/DeletionNotifier.h"
    #include "behaviours/Identifiable.h"
    #include "behaviours/Lockable.h"
    //#include "cryptography/SHA1.h"
    //#include "cryptography/SHA2.h"
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
    #include "networking/GoogleAnalyticsReporter.h"
    #include "networking/NetworkCache.h"
    #include "networking/NetworkConnectivityChecker.h"
    #include "networking/OAuth2.h"
    #include "networking/Product.h"
    #include "networking/User.h"
    #include "networking/Utilities.h"
    #include "networking/WebServiceUtilities.h"
    #include "networking/WooCommerce.h"
    #include "rng/BlumBlumShub.h"
    #include "rng/ISAAC.h"
    #include "rng/Xorshift.h"
    #include "squarepine_core.h"
    #include "text/LanguageHandler.h"
    #include "text/Utilities.h"
    #include "time/StopWatch.h"
    #include "unittests/SquarePineCoreUnitTestGatherer.h"
    #include "valuetree/JSONToValueTree.h"
    #include "valuetree/PropertyContainer.h"
}

#endif //SQUAREPINE_CORE_H

