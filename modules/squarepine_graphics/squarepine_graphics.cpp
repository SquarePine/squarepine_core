#include "squarepine_graphics.h"

namespace sp
{
    using namespace juce;

    std::unique_ptr<Drawable> createDrawableFromSVG (const File& file)
    {
        return svg::Parse::parse (file);
    }

    std::unique_ptr<Drawable> createDrawableFromSVG (const char* const data)
    {
        if (data != nullptr)
            if (auto d = XmlDocument::parse (String (data)))
                return svg::Parse::parse (*d);

        jassertfalse;
        return {};
    }

    Image createImageForDrawable (Drawable* drawable, int width, int height)
    {
        jassert (width > 0 && height > 0);

        Image image (Image::ARGB, width, height, true);
        Graphics g (image);

        drawable->drawWithin (g, Rectangle<int> (width, height).toFloat(), RectanglePlacement::centred, 1.0f);
        return image;
    }

    #include "application/SimpleApplication.cpp"
    //#include "audio/Automation.cpp"
    #include "components/ComponentViewer.cpp"
    #include "components/GoogleAnalyticsAttachment.cpp"
    #include "components/HighPerformanceRendererConfigurator.cpp"
    #include "components/ValueTreeEditor.cpp"
    #include "images/BlendingEffects.cpp"
    #include "images/BMPImageFormat.cpp"
    #include "images/ImageEffects.cpp"
    #include "images/ImageFormatManager.cpp"
    #include "images/StackBlurEffects.cpp"
    #include "images/SVGParser.cpp"
    #include "images/TGAImageFormat.cpp"
    //#include "images/WebPImageFormat.cpp"
    #include "linkers/CueSDKLinker.cpp"
    #include "lookandfeels/Windows10LookAndFeel.cpp"
   // #include "tokenisers/JavascriptCodeTokeniser.cpp"
}
