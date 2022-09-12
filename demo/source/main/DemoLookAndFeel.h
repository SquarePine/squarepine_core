namespace colours
{
    const auto darkest          = Colour (0xff111111);

    const auto grey             = Colour (0xffbfbfbf);
    const auto redSpace         = Colour (0xffe0007a);
    const auto purpleShift      = Colour (0xff65106b);
    const auto blueShape        = Colour (0xff009bca);
    const auto greenWet         = Colour (0xff00973d);
    const auto yellowDry        = Colour (0xffe4db00);
    const auto purpleIrGain     = Colour (0xff6f31c4);
    const auto yellowLimit      = Colour (0xffffaa46);
    const auto redClip          = Colour (0xffff465a);
    const auto mineshaft        = Colour (0xff302e2f);
    const auto buttonSVGOn      = Colours::white;
    const auto buttonSVGOff     = Colour (0xff464749);
    const auto blackout         = Colours::black.withAlpha (0.75f);
    const auto tundora          = Colour (0xff4d4d4d);

    namespace auth
    {
        const auto background           = Colour (0xff2c2c2c);
        const auto pinkish              = Colour (0xffe7328b);
        const auto pinkishBackground    = Colour (0xff3f2d36);
        const auto greyish              = Colour (0xff919191);
    }
}

//==============================================================================
/** */
class DemoLookAndFeel final : public LookAndFeel_V4
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
    // bool areScrollbarButtonsVisible() override { return true; }
    /** @internal */
    // void drawScrollbar (Graphics&, ScrollBar&, int, int, int, int, bool, int, int, bool, bool) override;
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
