#ifndef ZPLANE_HELPERS_H
#define ZPLANE_HELPERS_H

//==============================================================================
#if ! (JUCE_WINDOWS || JUCE_MAC)
    #undef SQUAREPINE_USE_AUFTAKT
    #undef SQUAREPINE_USE_ELASTIQUE
#endif

//==============================================================================
#if SQUAREPINE_USE_AUFTAKT || SQUAREPINE_USE_ELASTIQUE

    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Weverything", "-Wall", "-Wpedantic", "-Wno-eof-newline", "-Wnon-virtual-dtor")

    #if JUCE_WINDOWS
        #if SQUAREPINE_USE_AUFTAKT && ! defined (SQUAREPINE_AUFTAKT_PATH)
            #define SQUAREPINE_AUFTAKT_PATH    ../../../../sdks/auftakt/
        #endif

        #if SQUAREPINE_USE_ELASTIQUE && ! defined (SQUAREPINE_ELASTIQUE_PATH)
            #define SQUAREPINE_ELASTIQUE_PATH  ../../../../sdks/elastique/
        #endif

        #if JUCE_WINDOWS && _MSC_VER < 1800
            #error "Please update your tools."
            #error "Get over to https://visualstudio.microsoft.com/vs/community/"
        #endif

        #undef ZPLANE_MAKE_INCLUDE
        #define ZPLANE_MAKE_INCLUDE(x) \
                <x>

        #undef ZPLANE_INCLUDE
        #define ZPLANE_INCLUDE(a, b) \
                ZPLANE_MAKE_INCLUDE (JUCE_CONCAT (a, b))

        #if SQUAREPINE_USE_AUFTAKT
            #include ZPLANE_INCLUDE (SQUAREPINE_AUFTAKT_PATH, win/incl/aufTAKT_If.h)
            #include ZPLANE_INCLUDE (SQUAREPINE_AUFTAKT_PATH, win/incl/aufTAKTResultIf.h)
            #include ZPLANE_INCLUDE (SQUAREPINE_AUFTAKT_PATH, win/incl/zplAudioFile.h)
        #endif

        #if SQUAREPINE_USE_ELASTIQUE
            #include ZPLANE_INCLUDE (SQUAREPINE_ELASTIQUE_PATH, win/incl/elastiqueProV3API.h)
            #include ZPLANE_INCLUDE (SQUAREPINE_ELASTIQUE_PATH, win/incl/elastiqueProV3DirectAPI.h)
            #include ZPLANE_INCLUDE (SQUAREPINE_ELASTIQUE_PATH, win/incl/zplAudioFile.h)
        #endif

    #elif JUCE_MAC

        #if SQUAREPINE_USE_AUFTAKT
            #include "auftakt/macos/incl/aufTAKT_If.h"
            #include "auftakt/macos/incl/aufTAKTResultIf.h"
            #include "auftakt/macos/incl/zplAudioFile.h"
        #endif

        #if SQUAREPINE_USE_ELASTIQUE
            #include "elastique/macos/incl/elastiqueProV3API.h"
            #include "elastique/macos/incl/elastiqueProV3DirectAPI.h"
            #include "elastique/macos/incl/zplAudioFile.h"
        #endif

    #endif

    namespace zplane
    {
        //==============================================================================
        /** @returns true if the result code is an error.

            It appears that a result code of 0 (indicated as NULL, as in the C-style, ffs.)
            in the documentation is used to indicate a non-error/valid code.
        */
        inline constexpr bool isError (int resultCode) noexcept { return resultCode != 0; }

        /** @returns true if the result code is a success/non-error.
            Logs the error code and asserts on failure.
        */
        inline bool isValid (int resultCode)
        {
            if (isError (resultCode))
            {
                juce::String e = "Zplane library failed code: ";
                e << resultCode;
                juce::Logger::writeToLog (e);
                jassertfalse;
                return false;
            }

            return true;
        }
    } // zplane

    #include "AuftaktIncluder.h"
    #include "ElastiqueIncluder.h"

    JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#endif // SQUAREPINE_USE_AUFTAKT || SQUAREPINE_USE_ELASTIQUE

#endif // ZPLANE_HELPERS_H
