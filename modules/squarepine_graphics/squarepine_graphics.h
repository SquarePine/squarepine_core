#ifndef SQUAREPINE_GRAPHICS_H
#define SQUAREPINE_GRAPHICS_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_graphics
    vendor:             SquarePine
    version:            0.0.1
    name:               SquarePine Graphics
    description:        A grouping of reusable graphics classes, image formats, and other utilities.
    website:            https://www.squarepine.io
    license:            Proprietary
    minimumCppStandard: 11
    dependencies:       squarepine_core juce_gui_extra

    END_JUCE_MODULE_DECLARATION
*/
//==============================================================================
#include <squarepine_core/squarepine_core.h>
#include <juce_gui_extra/juce_gui_extra.h>

#if JUCE_MODULE_AVAILABLE_juce_opengl
    #include <juce_opengl/juce_opengl.h>
#else
    #undef JUCE_OPENGL
#endif

//==============================================================================
/** Config: SQUAREPINE_USE_AVIR_RESIZER
*/
#ifndef SQUAREPINE_USE_AVIR_RESIZER
    #define SQUAREPINE_USE_AVIR_RESIZER 0
#endif

/** Config: SQUAREPINE_USE_CUESDK

    If you're a fan of controlling RGB peripherals and want to
    control Corsair's devices, enable this on desktop to play around!

	Note: if you're using MinGW, you're SOL.
*/
#ifndef SQUAREPINE_USE_CUESDK
    #define SQUAREPINE_USE_CUESDK 0
#endif

#undef SQUAREPINE_USE_AVIR_RESIZER // Not yet supported...

//==============================================================================
namespace sp
{
    using namespace juce;

    /** */
    std::unique_ptr<Drawable> createDrawableFromSVG (const File& file);
    /** */
    std::unique_ptr<Drawable> createDrawableFromSVG (const char* const data);
    /** */
    Image createImageForDrawable (Drawable* drawable, int width, int height);

    /** */
    struct WaitCursorAutoHider final
    {
        WaitCursorAutoHider() { MouseCursor::showWaitCursor(); }
        ~WaitCursorAutoHider() { MouseCursor::hideWaitCursor(); }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaitCursorAutoHider)
    };

    namespace helpers
    {
        /** */
        inline void addAndMakeVisible (Component& parent, const Array<Component*>& children)
        {
            for (auto* c : children)
                parent.addAndMakeVisible (c);
        }

        /** */
        inline void addChildComponents (Component& parent, const Array<Component*>& children)
        {
            for (auto* c : children)
                parent.addChildComponent (c);
        }
    }

    /** */
    inline void drawFittedText (Graphics& g,
                                const String& text,
                                const Rectangle<float>& area,
                                Justification justification = Justification::centredLeft,
                                int maximumNumberOfLines = 1,
                                float minimumHorizontalScale = 1.0f)
    {
        if (text.isNotEmpty()
            && ! area.isEmpty()
            && g.clipRegionIntersects (area.getSmallestIntegerContainer()))
        {
            GlyphArrangement arr;
            arr.addFittedText (g.getCurrentFont(), text,
                               area.getX(), area.getY(),
                               area.getWidth(), area.getHeight(),
                               justification,
                               maximumNumberOfLines,
                               minimumHorizontalScale);
            arr.draw (g);
        }
    }

    /** */
    inline void drawFittedText (Graphics& g,
                                const String& text,
                                const Rectangle<int>& area,
                                Justification justification = Justification::centredLeft,
                                int maximumNumberOfLines = 1,
                                float minimumHorizontalScale = 1.0f)
    {
        drawFittedText (g, text, area.toFloat(), justification, maximumNumberOfLines, minimumHorizontalScale);
    }

    /** */
    class ValidPathCompatibleInputFilter : public TextEditor::InputFilter
    {
    public:
        /** */
        ValidPathCompatibleInputFilter (int maxStringLength = 128) :
            maxLength (maxStringLength)
        {
        }

        /** @internal */
        String filterNewText (TextEditor& ed, const String& newInput) override
        {
            auto t = newInput;

            // Anything but path-related stuff listed here:
            // https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file
            // And no escape char junk, nor path dividers.
            for (const auto c : U"<>:/|?*\"\\\'\?\a\b\f\n\r\t\v")
                t = t.replaceCharacter ((juce_wchar) c, ' ');

            return t.substring (0, maxLength - (ed.getTotalNumChars() - ed.getHighlightedRegion().getLength()));
        }

    private:
        const int maxLength = 128;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValidPathCompatibleInputFilter)
    };

    #include "application/SimpleApplication.h"
    //#include "audio/Automation.h"
    #include "components/ColourPropertyComponent.h"
    #include "components/ComponentViewer.h"
    #include "components/ComponentWindow.h"
    #include "components/Easing.h"
    #include "components/GoogleAnalyticsAttachment.h"
    #include "components/JavascriptEditor.h"
    #include "components/HighPerformanceRendererConfigurator.h"
    #include "components/ValueTreeEditor.h"
    #include "images/BlendingEffects.h"
    #include "images/BMPImageFormat.h"
    #include "images/ImageEffects.h"
    #include "images/ImageFormatManager.h"
    #include "images/SVGParser.h"
    #include "images/TGAImageFormat.h"
    //#include "images/WebPImageFormat.h"
    #include "linkers/CueSDKIncluder.h"
    #include "lookandfeels/Windows10LookAndFeel.h"
    //#include "tokenisers/JavascriptCodeTokeniser.h"
    #include "utilities/Fonts.h"
    #include "utilities/Particles.h"
    #include "utilities/Resolution.h"
}

#endif //SQUAREPINE_GRAPHICS_H
