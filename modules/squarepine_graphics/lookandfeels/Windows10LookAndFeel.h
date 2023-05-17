//==============================================================================
/** An all-encompassing look and feel that resembles Windows 10, as closely as possible.

    @see https://www.justinmind.com/ui-kits/windows-10-ui-kit
*/
class Windows10LookAndFeel final : public LookAndFeel_V4
{
public:
    Windows10LookAndFeel();

    //==============================================================================
    static constexpr auto topBarHeight = 48;
    static constexpr auto itemHeight = 40.0;
    static constexpr auto drawableFontHeight = 25.0f;
    static constexpr auto buttonIndent = 13;
    static constexpr auto toggleButtonIndent = 9;
    static constexpr auto tooltipFontSize = 16.0f;
    static constexpr auto maxTooltipWidth = 256;

    //==============================================================================
    /** */
    static void drawTextInRegion (Graphics& g, const Font& f, const String& text, Justification j,
                                  const juce::Rectangle<int>& region,
                                  Colour colour = Colours::darkgrey,
                                  bool isEnabled = true);

    /** */
    static Grid createDefaultGrid();

    //==============================================================================
    /** @internal */
    int getMenuWindowFlags() override { return 0; }
    /** @internal */
    int getAlertBoxWindowFlags() override { return ComponentPeer::windowAppearsOnTaskbar; }
    /** @internal */
    void playAlertSound() override { }
    /** @internal */
    Typeface::Ptr getTypefaceForFont (const Font& f) override;
    /** @internal */
    juce::Rectangle<int> getTooltipBounds (const String& tipText, Point<int> screenPos, juce::Rectangle<int> parentArea) override;
    /** @internal */
    void drawTooltip (Graphics&, const String& text, int width, int height) override;
    /** @internal */
    void drawLabel (Graphics&, Label&) override;
    /** @internal */
    Font getLabelFont (Label& label) override { return label.getFont(); }
    /** @internal */
    BorderSize<int> getLabelBorderSize (Label& label) override { return label.getBorderSize(); }
    /** @internal */
    int getTabButtonSpaceAroundImage() override { return 0; }
    /** @internal */
    int getTabButtonOverlap (int) override { return 0; }
    /** @internal */
    int getTabButtonBestWidth (TabBarButton&, int) override;
    /** @internal */
    juce::Rectangle<int> getTabButtonExtraComponentBounds (const TabBarButton&, juce::Rectangle<int>&, Component&) override;
    /** @internal */
    void drawTabButton (TabBarButton&, Graphics&, bool, bool) override;
    /** @internal */
    Font getTabButtonFont (TabBarButton&, float height) override { return { height * 0.75f }; }
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
    void drawButtonBackground (Graphics&, Button&, const Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    /** @internal */
    Font getTextButtonFont (TextButton&, int buttonHeight) override;
    /** @internal */
    int getTextButtonWidthToFitText (TextButton&, int buttonHeight) override;
    /** @internal */
    void drawButtonText (Graphics&, TextButton&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    /** @internal */
    void drawToggleButton (Graphics&, ToggleButton&,  bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    /** @internal */
    void changeToggleButtonWidthToFitText (ToggleButton&) override;
    /** @internal */
    void drawTickBox (Graphics&, Component&, float x, float y, float w, float h, bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    /** @internal */
    void drawDrawableButton (Graphics&, DrawableButton&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    /** @internal */
    void fillTextEditorBackground (Graphics&, int width, int height, TextEditor&) override;
    /** @internal */
    void drawTextEditorOutline (Graphics&, int width, int height, TextEditor&) override;
    /** @internal */
    float getCallOutBoxCornerSize (const CallOutBox&) override { return 0.0f; }
    /** @internal */
    void paintToolbarBackground (Graphics& g, int w, int h, Toolbar& toolbar) override;
    /** @internal */
    int getDefaultScrollbarWidth() override { return 16; }
    /** @internal */
    bool areScrollbarButtonsVisible() override { return true; }
    /** @internal */
    void drawScrollbar (Graphics&, ScrollBar&, int x, int y, int width, int height, bool isScrollbarVertical,
                        int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override;
    /** @internal */
    void drawPopupMenuBackground (Graphics& g, int width, int height) override;
    /** @internal */
    void drawMenuBarBackground (Graphics& g, int /*width*/, int /*height*/, bool, MenuBarComponent& menuBar) override;

private:
    //==============================================================================
    TextLayout layoutTooltipText (const String& text, Colour textColour) const;
    juce::Rectangle<float> getTooltipBoundsFloat (const String& tipText, Point<int> screenPos, juce::Rectangle<int> parentArea) const;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Windows10LookAndFeel)
};

