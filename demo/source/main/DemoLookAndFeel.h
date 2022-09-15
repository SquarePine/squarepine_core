//==============================================================================
namespace lightTheme
{
    const auto background       = Colours::white;
    const auto surface          = Colours::white;
    const auto error            = Colours::darkred;

    const auto primary          = Colour (0xff6200ee);
    const auto primaryVariant   = Colour (0xff3700b3);
    const auto secondary        = Colour (0xff03dac6);
    const auto secondaryVariant = Colour (0xff018786);

    const auto onBackground     = Colours::black;
    const auto onSurface        = Colours::white;
    const auto onError          = Colours::white;
    const auto onPrimary        = Colours::white;
    const auto onSecondary      = Colours::white;
}

//==============================================================================
namespace darkTheme
{
    const auto background       = Colours::white;
    const auto surface          = Colours::white;
    const auto error            = Colours::darkred;

    const auto primary          = Colour (0xff6200ee);
    const auto primaryVariant   = Colour (0xff3700b3);
    const auto secondary        = Colour (0xff03dac6);
    const auto secondaryVariant = Colour (0xff018786);

    const auto onBackground     = Colours::black;
    const auto onSurface        = Colours::white;
    const auto onError          = Colours::white;
    const auto onPrimary        = Colours::white;
    const auto onSecondary      = Colours::white;
}

//==============================================================================
/** */
class ColourScheme
{
public:
    /** */
    ColourScheme() = default;

    /** */
    virtual ~ColourScheme() = default;

    //==============================================================================
    /** */
    virtual void applyTo (LookAndFeel_V4& lookAndFeel) = 0;

    /** */
    virtual LookAndFeel_V4::ColourScheme create() = 0;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourScheme)
};

/** */
class LightColourScheme final : public ColourScheme
{
public:
    /** */
    LightColourScheme() = default;

    /** */
    void applyTo (LookAndFeel_V4& lookAndFeel) override
    {
        lookAndFeel.setColourScheme (create());

        lookAndFeel.setColour (CaretComponent::caretColourId,       lightTheme::onPrimary);
        lookAndFeel.setColour (Slider::textBoxOutlineColourId,      lightTheme::onPrimary);
        lookAndFeel.setColour (ScrollBar::thumbColourId,            lightTheme::onPrimary);
        lookAndFeel.setColour (TextButton::buttonColourId,          lightTheme::onPrimary);
        lookAndFeel.setColour (TextButton::buttonOnColourId,        lightTheme::onPrimary);
        lookAndFeel.setColour (TextButton::textColourOffId,         lightTheme::onPrimary);
        lookAndFeel.setColour (TextButton::textColourOnId,          lightTheme::onPrimary);
        lookAndFeel.setColour (TextEditor::backgroundColourId,      lightTheme::primary);
        lookAndFeel.setColour (TextEditor::focusedOutlineColourId,  lightTheme::onPrimary);
        lookAndFeel.setColour (TextEditor::highlightColourId,       lightTheme::onSecondary);
        lookAndFeel.setColour (TextEditor::highlightedTextColourId, lightTheme::onPrimary);
        lookAndFeel.setColour (TextEditor::outlineColourId,         lightTheme::onPrimary);
        lookAndFeel.setColour (TextEditor::shadowColourId,          Colours::transparentBlack);
        lookAndFeel.setColour (TooltipWindow::backgroundColourId,   lightTheme::primary);
        lookAndFeel.setColour (TooltipWindow::outlineColourId,      lightTheme::onPrimary);
        lookAndFeel.setColour (TooltipWindow::textColourId,         lightTheme::onPrimary);
        lookAndFeel.setColour (ListBox::backgroundColourId,         lightTheme::primary);
        lookAndFeel.setColour (ListBox::outlineColourId,            lightTheme::onPrimary);
        lookAndFeel.setColour (ListBox::textColourId,               lightTheme::onPrimary);
    }

    /** */
    LookAndFeel_V4::ColourScheme create() override
    {
        return
        {
            lightTheme::background,
            lightTheme::surface,
            lightTheme::background,
            lightTheme::primary,
            lightTheme::onPrimary,
            lightTheme::primary,
            lightTheme::onSecondary,
            lightTheme::secondary,
            lightTheme::onBackground
        };
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LightColourScheme)
};

/** */
class DarkColourScheme final : public ColourScheme
{
public:
    /** */
    DarkColourScheme() = default;

    /** */
    void applyTo (LookAndFeel_V4& lookAndFeel) override
    {
        lookAndFeel.setColourScheme (create());

        lookAndFeel.setColour (CaretComponent::caretColourId,       darkTheme::onPrimary);
        lookAndFeel.setColour (Slider::textBoxOutlineColourId,      darkTheme::onPrimary);
        lookAndFeel.setColour (ScrollBar::thumbColourId,            darkTheme::onPrimary);
        lookAndFeel.setColour (TextButton::buttonColourId,          darkTheme::onPrimary);
        lookAndFeel.setColour (TextButton::buttonOnColourId,        darkTheme::onPrimary);
        lookAndFeel.setColour (TextButton::textColourOffId,         darkTheme::onPrimary);
        lookAndFeel.setColour (TextButton::textColourOnId,          darkTheme::onPrimary);
        lookAndFeel.setColour (TextEditor::backgroundColourId,      darkTheme::primary);
        lookAndFeel.setColour (TextEditor::focusedOutlineColourId,  darkTheme::onPrimary);
        lookAndFeel.setColour (TextEditor::highlightColourId,       darkTheme::onSecondary);
        lookAndFeel.setColour (TextEditor::highlightedTextColourId, darkTheme::onPrimary);
        lookAndFeel.setColour (TextEditor::outlineColourId,         darkTheme::onPrimary);
        lookAndFeel.setColour (TextEditor::shadowColourId,          Colours::transparentBlack);
        lookAndFeel.setColour (TooltipWindow::backgroundColourId,   darkTheme::primary);
        lookAndFeel.setColour (TooltipWindow::outlineColourId,      darkTheme::onPrimary);
        lookAndFeel.setColour (TooltipWindow::textColourId,         darkTheme::onPrimary);
        lookAndFeel.setColour (ListBox::backgroundColourId,         darkTheme::primary);
        lookAndFeel.setColour (ListBox::outlineColourId,            darkTheme::onPrimary);
        lookAndFeel.setColour (ListBox::textColourId,               darkTheme::onPrimary);
    }

    /** */
    LookAndFeel_V4::ColourScheme create() override
    {
        return
        {
            darkTheme::background,
            darkTheme::surface,
            darkTheme::background,
            darkTheme::primary,
            darkTheme::onPrimary,
            darkTheme::primary,
            darkTheme::onSecondary,
            darkTheme::secondary,
            darkTheme::onBackground
        };
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DarkColourScheme)
};

//==============================================================================
/** */
class DemoLookAndFeel final : public LookAndFeel_V4,
                              public DarkModeSettingListener
{
public:
    //==============================================================================
    static constexpr auto margin = 28;
    static constexpr auto topBarHeight = 120;
    static constexpr auto itemHeight = 38;
    static constexpr auto defaultFontHeight = 16.0f;
    static constexpr auto drawableFontHeight = 25.0f;
    static constexpr auto buttonIndent = 13;
    static constexpr auto toggleButtonIndent = 9;
    static constexpr auto tooltipFontSize = defaultFontHeight;
    static constexpr auto maxTooltipWidth = 256;
    static constexpr auto corner = MathConstants<float>::pi;

    //==============================================================================
    /** */
    DemoLookAndFeel (SharedObjects&);

    /** */
    ~DemoLookAndFeel() override;

    //==============================================================================
    /** */
    static void drawTextInRegion (Graphics& g, const Font& f, const String& text, Justification j,
                                  const Rectangle<int>& region, Colour colour = Colours::darkgrey, bool isEnabled = true);

    /** */
    static Grid createDefaultGrid();

    //==============================================================================
    /** @internal */
    void darkModeSettingChanged() override;
    /** @internal */
    int getMenuWindowFlags() override { return 0; }
    /** @internal */
    int getDefaultMenuBarHeight() override { return roundToInt (defaultFontHeight * 1.5f); }
    /** @internal */
    Font getMenuBarFont (MenuBarComponent&, int, const String&) override { return defaultFontHeight; }
    /** @internal */
    void drawMenuBarBackground (Graphics&, int, int, bool, MenuBarComponent&) override;
    /** @internal */
    void drawMenuBarItem (Graphics&, int, int, int, const String&, bool, bool, bool, MenuBarComponent&) override;
    /** @internal */
    int getAlertBoxWindowFlags() override { return ComponentPeer::windowAppearsOnTaskbar; }
    /** @internal */
    void playAlertSound() override { }
    /** @internal */
    Typeface::Ptr getTypefaceForFont (const Font& f) override;
    /** @internal */
    Rectangle<int> getTooltipBounds (const String&, Point<int>, Rectangle<int>) override;
    /** @internal */
    void drawTooltip (Graphics&, const String&, int, int) override;
    /** @internal */
    Font getPopupMenuFont() override { return defaultFontHeight; }
    /** @internal */
    void drawLabel (Graphics&, Label&) override;
    /** @internal */
    Font getLabelFont (Label& label) override { return label.getFont(); }
    /** @internal */
    BorderSize<int> getLabelBorderSize (Label&) override { return {}; }
    /** @internal */
    int getTabButtonSpaceAroundImage() override { return 0; }
    /** @internal */
    int getTabButtonOverlap (int) override { return 0; }
    /** @internal */
    int getTabButtonBestWidth (TabBarButton&, int) override;
    /** @internal */
    Rectangle<int> getTabButtonExtraComponentBounds (const TabBarButton&, Rectangle<int>&, Component&) override;
    /** @internal */
    void drawTabButton (TabBarButton&, Graphics&, bool, bool) override;
    /** @internal */
    Font getTabButtonFont (TabBarButton&, float) override { return defaultFontHeight; }
    /** @internal */
    void drawTabButtonText (TabBarButton&, Graphics&, bool, bool) override;
    /** @internal */
    void drawTabbedButtonBarBackground (TabbedButtonBar&, Graphics&) override { }
    /** @internal */
    void drawTabAreaBehindFrontButton (TabbedButtonBar&, Graphics&, int, int) override { }
    /** @internal */
    void createTabButtonShape (TabBarButton&, Path&,  bool, bool) override;
    /** @internal */
    void fillTabButtonShape (TabBarButton&, Graphics&, const Path&, bool, bool) override;
    /** @internal */
    Button* createTabBarExtrasButton() override;
    /** @internal */
    void drawComboBox (Graphics&, int, int, bool, int, int, int, int, ComboBox&) override;
    /** @internal */
    void drawButtonBackground (Graphics&, Button&, const Colour&, bool, bool) override;
    /** @internal */
    Font getTextButtonFont (TextButton&, int) override;
    /** @internal */
    int getTextButtonWidthToFitText (TextButton&, int) override;
    /** @internal */
    void drawButtonText (Graphics&, TextButton&, bool, bool) override;
    /** @internal */
    void drawToggleButton (Graphics&, ToggleButton&, bool, bool) override;
    /** @internal */
    void changeToggleButtonWidthToFitText (ToggleButton&) override;
    /** @internal */
    void drawTickBox (Graphics&, Component&, float, float, float, float, bool, bool, bool, bool) override;
    /** @internal */
    void drawDrawableButton (Graphics&, DrawableButton&, bool, bool) override;
    /** @internal */
    void fillTextEditorBackground (Graphics&, int, int, TextEditor&) override;
    /** @internal */
    void drawTextEditorOutline (Graphics&, int, int, TextEditor&) override;
    /** @internal */
    float getCallOutBoxCornerSize (const CallOutBox&) override { return corner; }
    /** @internal */
    void paintToolbarBackground (Graphics&, int, int, Toolbar&) override;
    /** @internal */
    int getDefaultScrollbarWidth() override { return 14; }
    /** @internal */
    bool areScrollbarButtonsVisible() override { return false; }
    /** @internal */
    void drawScrollbar (Graphics&, ScrollBar&, int, int, int, int, bool, int, int, bool, bool) override;
    /** @internal */
    void drawPopupMenuBackground (Graphics&, int, int) override;
    /** @internal */
    void drawStretchableLayoutResizerBar (Graphics&, int, int, bool, bool, bool) override;

private:
    //==============================================================================
    SharedObjects& sharedObjects;

    //==============================================================================
    TextLayout layoutTooltipText (const String& text, Colour textColour) const;
    Rectangle<float> getTooltipBoundsFloat (const String& tipText, Point<int> screenPos, Rectangle<int> parentArea) const;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoLookAndFeel)
};
