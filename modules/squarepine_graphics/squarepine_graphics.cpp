#define JUCE_CORE_INCLUDE_NATIVE_HEADERS _WINDOWS

#include "squarepine_graphics.h"

#if JUCE_MODULE_AVAILABLE_squarepine_images
    #include <squarepine_images/squarepine_images.h>
#endif

#if JUCE_ANDROID
    #include <sys/system_properties.h>
    #include <android/api-level.h>
#endif

#include "lighting/iCUESDKLinker.cpp"

namespace sp
{
    using namespace juce;

    #include "application/SimpleApplication.cpp"
    #include "components/ComponentViewer.cpp"
    #include "components/GoogleAnalyticsAttachment.cpp"
    #include "components/HighPerformanceRendererConfigurator.cpp"
    #include "components/ValueTreeEditor.cpp"
    #include "images/BlendingEffects.cpp"
    #include "images/BMPImageFormat.cpp"
    #include "images/DrawableHelpers.cpp"
    #include "images/ImageEffects.cpp"
    #include "images/ImageFormatManager.cpp"
    #include "images/Resizer.cpp"
    #include "images/StackBlurEffects.cpp"
    #include "images/SVGParser.cpp"
    #include "images/TGAImageFormat.cpp"
    #include "lookandfeels/Windows10LookAndFeel.cpp"
   // #include "tokenisers/JavascriptCodeTokeniser.cpp"
}

#include "lighting/WinRTRGB.cpp"
