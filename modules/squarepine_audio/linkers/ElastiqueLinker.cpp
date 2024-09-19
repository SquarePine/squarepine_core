#if SQUAREPINE_USE_ELASTIQUE && JUCE_MSVC

    #undef SQUAREPINE_ELASTIQUE_BIT_PATH
   #if JUCE_64BIT
    #define SQUAREPINE_ELASTIQUE_BIT_PATH "x64/"
   #else
    #define SQUAREPINE_ELASTIQUE_BIT_PATH "x86/"
   #endif

    #undef SQUAREPINE_ELASTIQUE_ROOT_PATH
    #define SQUAREPINE_ELASTIQUE_ROOT_PATH "../../sdks/elastique/win/lib/" SQUAREPINE_ELASTIQUE_BIT_PATH "NONE/"

    #undef SQUAREPINE_ELASTIQUE_VC_PATH

   #if _MSC_VER >= 1900
    #define SQUAREPINE_ELASTIQUE_VC_PATH "VC16/"
   #elif _MSC_VER >= 1800
    #define SQUAREPINE_ELASTIQUE_VC_PATH "VC15/"
   #endif

    #undef SQUAREPINE_ELASTIQUE_DR_PATH
   #if JUCE_DEBUG
    #define SQUAREPINE_ELASTIQUE_DR_PATH "Debug/"
   #else
    #define SQUAREPINE_ELASTIQUE_DR_PATH "Release/"
   #endif

    #undef SQUAREPINE_ELASTIQUE_FULL_PATH

    // Should basically look something like this: [...]/sdks/elastique/win/lib/x86/NONE/VC16/MT/Debug/
    #define SQUAREPINE_ELASTIQUE_FULL_PATH \
        SQUAREPINE_ELASTIQUE_ROOT_PATH SQUAREPINE_ELASTIQUE_VC_PATH "MD/" SQUAREPINE_ELASTIQUE_DR_PATH

    #pragma comment (lib, SQUAREPINE_ELASTIQUE_FULL_PATH "libelastiqueEfficientV3.lib")
    #pragma comment (lib, SQUAREPINE_ELASTIQUE_FULL_PATH "libelastiqueProV3.lib")
    #pragma comment (lib, SQUAREPINE_ELASTIQUE_FULL_PATH "libelastiqueSOLOIST.lib")
    #pragma comment (lib, SQUAREPINE_ELASTIQUE_FULL_PATH "libResample.lib")
    #pragma comment (lib, SQUAREPINE_ELASTIQUE_FULL_PATH "libzplAudioFile.lib")
    #pragma comment (lib, SQUAREPINE_ELASTIQUE_FULL_PATH "libzplVecLib.lib")

#endif // SQUAREPINE_USE_ELASTIQUE
