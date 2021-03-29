#if SQUAREPINE_USE_CUESDK && JUCE_MSVC && ! JUCE_MINGW

    #undef SQUAREPINE_CUESDK_ROOT_PATH
   #if JUCE_64BIT
    #define SQUAREPINE_CUESDK_ROOT_PATH "../../sdks/CUESDK/lib/x64/CUESDK.x64_"
   #else
    #define SQUAREPINE_CUESDK_ROOT_PATH "../../sdks/CUESDK/lib/i386/CUESDK_"
   #endif

    #undef SQUAREPINE_CUESDK_VC_PATH

   #if _MSC_VER >= 1800
    #define SQUAREPINE_CUESDK_VC_PATH "2017"
   #else
    #error "Forget it, you stingy developer. Update your tools."
    #error "Get your ass over to https://visualstudio.microsoft.com/vs/community/"
   #endif

    // Should basically look something like this: [...]/sdks/CUESDK/lib/i386/CUESDK_2017.lib
    #define SQUAREPINE_CUESDK_FULL_PATH \
        SQUAREPINE_CUESDK_ROOT_PATH SQUAREPINE_CUESDK_VC_PATH ".lib"

    #pragma comment (lib, SQUAREPINE_CUESDK_FULL_PATH)

#endif // SQUAREPINE_USE_CUESDK
