#if SQUAREPINE_USE_AUFTAKT && JUCE_WINDOWS

    #if ! defined (SQUAREPINE_AUFTAKT_LINK_PATH)
        #define SQUAREPINE_AUFTAKT_LINK_PATH "../../sdks/auftakt/"
    #endif

    #undef SQUAREPINE_AUFTAKT_BIT_PATH
   #if JUCE_64BIT
    #define SQUAREPINE_AUFTAKT_BIT_PATH "x64/"
   #else
    #define SQUAREPINE_AUFTAKT_BIT_PATH "x86/"
   #endif

    #undef SQUAREPINE_AUFTAKT_ROOT_PATH
    #define SQUAREPINE_AUFTAKT_ROOT_PATH SQUAREPINE_AUFTAKT_LINK_PATH "win/lib/" SQUAREPINE_AUFTAKT_BIT_PATH "NONE/"

    #undef SQUAREPINE_AUFTAKT_VC_PATH

   #if _MSC_VER >= 1900
    #define SQUAREPINE_AUFTAKT_VC_PATH "VC16/"
   #elif _MSC_VER >= 1800
    #define SQUAREPINE_AUFTAKT_VC_PATH "VC15/"
   #endif

    #undef SQUAREPINE_AUFTAKT_DR_PATH
   #if JUCE_DEBUG
    #define SQUAREPINE_AUFTAKT_DR_PATH "Debug/"
   #else
    #define SQUAREPINE_AUFTAKT_DR_PATH "Release/"
   #endif

    #undef SQUAREPINE_AUFTAKT_FULL_PATH

    // Should basically look something like this: [...]/sdks/auftakt/win/lib/x86/NONE/VC16/MT/Debug/
    #define SQUAREPINE_AUFTAKT_FULL_PATH \
        SQUAREPINE_AUFTAKT_ROOT_PATH SQUAREPINE_AUFTAKT_VC_PATH "MD/" SQUAREPINE_AUFTAKT_DR_PATH

    #pragma comment (lib, SQUAREPINE_AUFTAKT_FULL_PATH "libaufTAKT.lib")
    #pragma comment (lib, SQUAREPINE_AUFTAKT_FULL_PATH "libaufTAKTResult.lib")
    #pragma comment (lib, SQUAREPINE_AUFTAKT_FULL_PATH "libfxpack.lib")
    #pragma comment (lib, SQUAREPINE_AUFTAKT_FULL_PATH "libOnsetTracking.lib")
    #pragma comment (lib, SQUAREPINE_AUFTAKT_FULL_PATH "libzplAudioFile.lib")
    #pragma comment (lib, SQUAREPINE_AUFTAKT_FULL_PATH "libzplVecLib.lib")

#endif // SQUAREPINE_USE_AUFTAKT
