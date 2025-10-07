#define JUCE_CORE_INCLUDE_NATIVE_HEADERS _WINDOWS

#include "squarepine_graphics.h"

#if JUCE_MODULE_AVAILABLE_squarepine_images
    #include <squarepine_images/squarepine_images.h>
#endif

#if JUCE_ANDROID
    #include <sys/system_properties.h>
    #include <android/api-level.h>
#endif

#include "lighting/squarepine_iCUESDKLinker.cpp"
#include "lighting/squarepine_WinRTRGB.cpp"

namespace sp
{
    using namespace juce;

    #include "application/squarepine_SimpleApplication.cpp"
    #include "components/squarepine_ComponentViewer.cpp"
    #include "components/squarepine_GoogleAnalyticsAttachment.cpp"
    #include "components/squarepine_HighPerformanceRendererConfigurator.cpp"
    #include "components/squarepine_MarkdownComponent.cpp"
    #include "components/squarepine_ValueTreeEditor.cpp"
    #include "images/squarepine_BlendingEffects.cpp"
    #include "images/squarepine_BMPImageFormat.cpp"
    #include "images/squarepine_DrawableHelpers.cpp"
    #include "images/squarepine_ImageEffects.cpp"
    #include "images/squarepine_ImageFormatManager.cpp"
    #include "images/squarepine_Resizer.cpp"
    #include "images/squarepine_StackBlurEffects.cpp"
    #include "images/squarepine_SVGParser.cpp"
    #include "images/squarepine_TGAImageFormat.cpp"
    #include "lookandfeels/squarepine_Windows10LookAndFeel.cpp"
   // #include "tokenisers/squarepine_JavascriptCodeTokeniser.cpp"
}
