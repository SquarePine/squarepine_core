#ifndef SQUAREPINE_GRAPHICS_H
#define SQUAREPINE_GRAPHICS_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_graphics
    vendor:             SquarePine
    version:            1.7.0
    name:               SquarePine Graphics
    description:        A group of reusable graphics classes, image formats, Components, and related utilities.
    website:            https://www.squarepine.io
    license:            GPLv3
    minimumCppStandard: 20
    dependencies:       squarepine_cryptography juce_gui_extra

    END_JUCE_MODULE_DECLARATION
*/
//==============================================================================
#include <squarepine_core/squarepine_core.h>
#include <juce_gui_extra/juce_gui_extra.h>

//==============================================================================
/** Config: SQUAREPINE_USE_AVIR_RESIZER

    A high-quality image resizer.

    By default, this is enabled when building against Intel CPU systems.
*/
#ifndef SQUAREPINE_USE_AVIR_RESIZER
    #define SQUAREPINE_USE_AVIR_RESIZER JUCE_INTEL
#endif

/** Config: SQUAREPINE_USE_ICUESDK

    If you're a fan of controlling RGB peripherals and want to control
    Corsair's devices, enable this to play around on mainstream desktops.

    Only available for 64-bit Windows and macOS (Intel only).
*/
#ifndef SQUAREPINE_USE_ICUESDK
    #define SQUAREPINE_USE_ICUESDK 0
#endif

/** Config: SQUAREPINE_LOG_OPENGL_INFO
*/
#ifndef SQUAREPINE_LOG_OPENGL_INFO
    #define SQUAREPINE_LOG_OPENGL_INFO 0
#endif

/** Config: SQUAREPINE_USE_WINRTRGB
*/
#ifndef SQUAREPINE_USE_WINRTRGB
    #define SQUAREPINE_USE_WINRTRGB 1
#endif

//==============================================================================
#if ! JUCE_WINDOWS
    #undef SQUAREPINE_USE_WINRTRGB
#endif

//==============================================================================
#include "lighting/squarepine_iCUESDKIncluder.h"

//==============================================================================
namespace sp
{
    using namespace juce;

    /** */
    class WaitCursorAutoHider final
    {
    public:
        WaitCursorAutoHider() { MouseCursor::showWaitCursor(); }
        ~WaitCursorAutoHider() { MouseCursor::hideWaitCursor(); }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaitCursorAutoHider)
    };

    /** */
    class ScreenSaverAutoDisabler final
    {
    public:
        ScreenSaverAutoDisabler() { Desktop::getInstance().setScreenSaverEnabled (false); }
        ~ScreenSaverAutoDisabler() { Desktop::getInstance().setScreenSaverEnabled (true); }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScreenSaverAutoDisabler)
    };

    namespace helpers
    {
        /** */
        inline void addAndMakeVisible (Component& parent, const std::vector<Component*>& children)
        {
            for (auto* c : children)
                parent.addAndMakeVisible (c);
        }

        /** */
        inline void addChildComponents (Component& parent, const std::vector<Component*>& children)
        {
            for (auto* c : children)
                parent.addChildComponent (c);
        }
    }

    /** */
    inline void drawFittedText (Graphics& g,
                                const String& text,
                                const juce::Rectangle<float>& area,
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
                                const juce::Rectangle<int>& area,
                                Justification justification = Justification::centredLeft,
                                int maximumNumberOfLines = 1,
                                float minimumHorizontalScale = 1.0f)
    {
        drawFittedText (g, text, area.toFloat(), justification, maximumNumberOfLines, minimumHorizontalScale);
    }

    /** */
    class ValidPathCompatibleInputFilter final : public TextEditor::InputFilter
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

    //==============================================================================
    /** Why waste memory and cycles reallocating a RectangleList when you can just
        forcibly set all of the rectangles to a new height instead?
    */
    template<typename ValueType>
    inline void updateWithNewHeight (RectangleList<ValueType>& rectList, ValueType newHeight) noexcept
    {
        for (auto& r : rectList)
            const_cast<juce::Rectangle<ValueType>&> (r).setHeight (newHeight);
    }

    /** Why waste memory and cycles reallocating a RectangleList when you can just
        forcibly set all of the rectangles to a new width instead?
    */
    template<typename ValueType>
    inline void updateWithNewWidth (RectangleList<ValueType>& rectList, ValueType newWidth) noexcept
    {
        for (auto& r : rectList)
            const_cast<juce::Rectangle<ValueType>&> (r).setWidth (newWidth);
    }

    //==============================================================================
    #include "application/squarepine_SimpleApplication.h"
    #include "components/squarepine_PropertyComponents.h"
    #include "components/squarepine_ComponentViewer.h"
    #include "components/squarepine_ComponentWindow.h"
    #include "components/squarepine_GoogleAnalyticsAttachment.h"
    #include "components/squarepine_JavascriptEditor.h"
    #include "components/squarepine_HighPerformanceRendererConfigurator.h"
    #include "components/squarepine_MarkdownComponent.h"
    #include "components/squarepine_ValueTreeEditor.h"
    #include "images/squarepine_BlendingEffects.h"
    #include "images/squarepine_BMPImageFormat.h"
    #include "images/squarepine_DrawableHelpers.h"
    #include "images/squarepine_ImageEffects.h"
    #include "images/squarepine_ImageFormatManager.h"
    #include "images/squarepine_Resizer.h"
    #include "images/squarepine_SVGParser.h"
    #include "images/squarepine_TGAImageFormat.h"
    //#include "images/WebPImageFormat.h"
    #include "lighting/squarepine_WinRTRGB.h"
    #include "lookandfeels/squarepine_Windows10LookAndFeel.h"
    //#include "tokenisers/JavascriptCodeTokeniser.h"
    #include "utilities/squarepine_Fonts.h"
    #include "utilities/squarepine_Resolution.h"
}

#endif //SQUAREPINE_GRAPHICS_H
