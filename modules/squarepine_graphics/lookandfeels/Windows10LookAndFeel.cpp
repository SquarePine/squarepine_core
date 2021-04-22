//==============================================================================
namespace Win10Colours
{
    const Colour background = Colour (253, 253, 253);
    const Colour blue       = Colour (0, 120, 215);
    const Colour text       = Colours::black;
}

//==============================================================================
Windows10LookAndFeel::Windows10LookAndFeel()
{
    Font::setDefaultMinimumHorizontalScaleFactor (1.0f);
    Font::setFallbackFontName ("Arial");
    setDefaultSansSerifTypefaceName ("Segoe");

    setColourScheme
    ({
        Win10Colours::background,   Win10Colours::background,   Win10Colours::background,
        Colour (209, 209, 209),     Win10Colours::text,         Colour (209, 209, 209),
        Colours::white,             Win10Colours::blue,         Win10Colours::text
    });

/*
    setColour (CodeEditorComponent::backgroundColourId,     Win10Colours::background);
    setColour (CodeEditorComponent::defaultTextColourId,    Colours::black);
    setColour (CodeEditorComponent::highlightColourId,      Colours::blue);
    setColour (CodeEditorComponent::lineNumberBackgroundId, Colours::white);
    setColour (CodeEditorComponent::lineNumberTextId,       Colours::black);
    setColour (DrawableButton::backgroundColourId,          Colours::white);
    setColour (DrawableButton::backgroundOnColourId,        Colours::white);
    setColour (HyperlinkButton::textColourId,               Colour (0, 120, 215));
    setColour (ResizableWindow::backgroundColourId,         Win10Colours::background);
    setColour (Slider::backgroundColourId,                  Colours::white);
    setColour (Slider::thumbColourId,                       Colour (0, 120, 215));
    setColour (Slider::trackColourId,                       Colours::white);
    setColour (TextEditor::backgroundColourId,              Colours::white);
    setColour (TextEditor::focusedOutlineColourId,          Colours::white);
    setColour (TextEditor::outlineColourId,                 Colours::black);
    setColour (TextEditor::textColourId,                    Colours::black);
    setColour (Toolbar::backgroundColourId,                 Colours::white);
    setColour (Toolbar::buttonMouseDownBackgroundColourId,  Colours::white);
    setColour (Toolbar::separatorColourId,                  Colours::white);
    setColour (TooltipWindow::backgroundColourId,           Colour (242, 242, 242));
    setColour (TooltipWindow::outlineColourId,              Colour (209, 209, 209));
    setColour (TooltipWindow::textColourId,                 Colours::black);
*/
}

//==============================================================================
Typeface::Ptr Windows10LookAndFeel::getTypefaceForFont (const Font& f)
{
    auto copyFontWithName = [&] (const String& typefaceName)
    {
        Font copy (f);
        copy.setTypefaceName (typefaceName);
        return Typeface::createSystemTypefaceFor (copy);
    };

    const auto& typefaceName = f.getTypefaceName();

    if (typefaceName == Font::getDefaultSansSerifFontName())
        return copyFontWithName ("Segoe");
    else if (typefaceName == Font::getDefaultSerifFontName())
        return copyFontWithName ("Segoe");
    else if (typefaceName == Font::getDefaultMonospacedFontName())
        return copyFontWithName ("Courier New");

    return LookAndFeel_V4::getTypefaceForFont (f);
}

//==============================================================================
TextLayout Windows10LookAndFeel::layoutTooltipText (const String& text, Colour textColour) const
{
    if (text.isEmpty())
        return {};

    AttributedString s;
    s.setJustification (Justification::topLeft);
    s.append (text, Font (tooltipFontSize), textColour);

    TextLayout tl;
    tl.createLayoutWithBalancedLineLengths (s, (float) maxTooltipWidth);
    return tl;
}

Rectangle<float> Windows10LookAndFeel::getTooltipBoundsFloat (const String& tipText, Point<int> screenPos, Rectangle<int> parentArea) const
{
    const auto tl = layoutTooltipText (tipText, Colours::black);

    return Rectangle<float> (tl.getWidth(), tl.getHeight())
            .withPosition (screenPos.toFloat())
            .constrainedWithin (parentArea.toFloat());
}

Rectangle<int> Windows10LookAndFeel::getTooltipBounds (const String& tipText, Point<int> screenPos, Rectangle<int> parentArea)
{
    return getTooltipBoundsFloat (tipText, screenPos, parentArea)
            .getSmallestIntegerContainer();
}

void Windows10LookAndFeel::drawTooltip (Graphics& g, const String& text, int w, int h)
{
    if (text.isEmpty())
        return;

    const auto bounds = Rectangle<int> (w, h).toFloat();

    const auto bgColour = findColour (TooltipWindow::backgroundColourId);
    jassert (bgColour.isOpaque());
    g.setColour (bgColour);
    g.fillRect (bounds);

    const auto textColour = findColour (TooltipWindow::textColourId);
    jassert (textColour.isOpaque());

    layoutTooltipText (text, textColour).draw (g, bounds);

    const auto outlineColour = findColour (TooltipWindow::outlineColourId);
    if (! outlineColour.isTransparent())
    {
        g.setColour (outlineColour);
        g.drawRect (bounds);
    }
}

//==============================================================================
int Windows10LookAndFeel::getTabButtonBestWidth (TabBarButton& button, int tabDepth)
{
    auto width = Font ((float) tabDepth).getStringWidth (button.getButtonText().trim())
                 + getTabButtonOverlap (tabDepth) * 2;

    if (auto* extraComponent = button.getExtraComponent())
        width += button.getTabbedButtonBar().isVertical() ? extraComponent->getHeight()
                                                          : extraComponent->getWidth();

    return jlimit (tabDepth * 2, tabDepth * 8, width);
}

void Windows10LookAndFeel::drawTextInRegion (Graphics& g, const Font& font, const String& text, Justification j,
                                        const Rectangle<int>& region, Colour colour, bool isEnabled)
{
    g.setColour (colour.withMultipliedAlpha (isEnabled ? 1.0f : 0.5f));
    g.setFont (font);

    drawFittedText (g, text, region, j, roundToIntAccurate ((double) region.getHeight() / (double) font.getHeight()));
}

Grid Windows10LookAndFeel::createDefaultGrid()
{
    Grid grid;
    grid.autoRows = grid.autoColumns = Grid::TrackInfo (1_fr);
    return grid;
}

void Windows10LookAndFeel::drawLabel (Graphics& g, Label& label)
{
    g.fillAll (label.findColour (Label::backgroundColourId));

    if (! label.isBeingEdited())
    {
        const auto alpha = label.isEnabled() ? 1.0f : 0.5f;

        const auto textArea = getLabelBorderSize (label).subtractedFrom (label.getLocalBounds());
        drawTextInRegion (g, getLabelFont (label), label.getText(), label.getJustificationType(),
                          textArea, label.findColour (Label::textColourId), label.isEnabled());

        g.setColour (label.findColour (Label::outlineColourId).withMultipliedAlpha (alpha));
    }
    else if (label.isEnabled())
    {
        g.setColour (label.findColour (Label::outlineColourId));
    }

    g.drawRect (label.getLocalBounds());
}

Rectangle<int> Windows10LookAndFeel::getTabButtonExtraComponentBounds (const TabBarButton& button, Rectangle<int>& textArea, Component& comp)
{
    Rectangle<int> extraComp;

    const auto orientation = button.getTabbedButtonBar().getOrientation();

    if (button.getExtraComponentPlacement() == TabBarButton::beforeText)
    {
        switch (orientation)
        {
            case TabbedButtonBar::TabsAtBottom:
            case TabbedButtonBar::TabsAtTop:     extraComp = textArea.removeFromLeft (comp.getWidth()); break;
            case TabbedButtonBar::TabsAtLeft:    extraComp = textArea.removeFromBottom (comp.getHeight()); break;
            case TabbedButtonBar::TabsAtRight:   extraComp = textArea.removeFromTop (comp.getHeight()); break;
            default:                             jassertfalse; break;
        }
    }
    else
    {
        switch (orientation)
        {
            case TabbedButtonBar::TabsAtBottom:
            case TabbedButtonBar::TabsAtTop:     extraComp = textArea.removeFromRight (comp.getWidth()); break;
            case TabbedButtonBar::TabsAtLeft:    extraComp = textArea.removeFromTop (comp.getHeight()); break;
            case TabbedButtonBar::TabsAtRight:   extraComp = textArea.removeFromBottom (comp.getHeight()); break;
            default:                             jassertfalse; break;
        }
    }

    return extraComp;
}

void Windows10LookAndFeel::createTabButtonShape (TabBarButton& button, Path& p, bool, bool)
{
    p.addRectangle (button.getLocalBounds().toFloat());
}

void Windows10LookAndFeel::fillTabButtonShape (TabBarButton& button, Graphics& g,
                                               const Path& path, bool over, bool)
{
    auto c = Colour (246, 246, 246);

    if (over)
        c = c.brighter();

    g.setColour (c);
    g.fillPath (path);

    if (button.isFrontTab())
    {
        g.setColour (findColour (HyperlinkButton::textColourId));
        g.drawRect (button.getBounds().reduced (3).removeFromLeft (3));
    }
}

void Windows10LookAndFeel::drawTabButtonText (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
{
    auto area = button.getTextArea().toFloat();
    auto length = area.getWidth();
    auto depth  = area.getHeight();

    if (button.getTabbedButtonBar().isVertical())
        std::swap (length, depth);

    auto font = getTabButtonFont (button, depth);
    font.setUnderline (button.hasKeyboardFocus (false));

    AffineTransform t;

    switch (button.getTabbedButtonBar().getOrientation())
    {
        case TabbedButtonBar::TabsAtLeft:   t = t.rotated (MathConstants<float>::pi * -0.5f).translated (area.getX(), area.getBottom()); break;
        case TabbedButtonBar::TabsAtRight:  t = t.rotated (MathConstants<float>::pi *  0.5f).translated (area.getRight(), area.getY()); break;

        case TabbedButtonBar::TabsAtTop:
        case TabbedButtonBar::TabsAtBottom:
            t = t.translated (area.getX(), area.getY());
        break;

        default: jassertfalse; break;
    }

    Colour c;

    if (button.isFrontTab() && (button.isColourSpecified (TabbedButtonBar::frontTextColourId) || isColourSpecified (TabbedButtonBar::frontTextColourId)))
        c = findColour (TabbedButtonBar::frontTextColourId);
    else if (button.isColourSpecified (TabbedButtonBar::tabTextColourId) || isColourSpecified (TabbedButtonBar::tabTextColourId))
        c = findColour (TabbedButtonBar::tabTextColourId);
    else
        c = button.getTabBackgroundColour().contrasting();

    const auto alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;

    g.setColour (c.withMultipliedAlpha (alpha));
    g.setFont (font);
    g.addTransform (t);

    drawFittedText (g, button.getButtonText(),
                    Rectangle<float> (length, depth),
                    Justification::centred,
                    roundToInt (jmax (1.0f, depth / 12.0f)));
}

void Windows10LookAndFeel::drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
{
    Path tabShape;
    createTabButtonShape (button, tabShape, isMouseOver, isMouseDown);
    fillTabButtonShape (button, g, tabShape, isMouseOver, isMouseDown);
    drawTabButtonText (button, g, isMouseOver, isMouseDown);
}

Button* Windows10LookAndFeel::createTabBarExtrasButton()
{
    constexpr auto thickness = 7.0f;
    constexpr auto indent = 22.0f;

    Path p;
    p.addEllipse (-10.0f, -10.0f, 120.0f, 120.0f);

    DrawablePath ellipse;
    ellipse.setPath (p);
    ellipse.setFill (Colour (0x99ffffff));

    p.clear();
    p.addEllipse (0.0f, 0.0f, 100.0f, 100.0f);
    p.addRectangle (indent, 50.0f - thickness, 100.0f - indent * 2.0f, thickness * 2.0f);
    p.addRectangle (50.0f - thickness, indent, thickness * 2.0f, 50.0f - indent - thickness);
    p.addRectangle (50.0f - thickness, 50.0f + thickness, thickness * 2.0f, 50.0f - indent - thickness);
    p.setUsingNonZeroWinding (false);

    DrawablePath dp;
    dp.setPath (p);
    dp.setFill (Colour (0x59000000));

    DrawableComposite normalImage;
    normalImage.addAndMakeVisible (ellipse.createCopy().release());
    normalImage.addAndMakeVisible (dp.createCopy().release());

    dp.setFill (Colour (0xcc000000));

    DrawableComposite overImage;
    overImage.addAndMakeVisible (ellipse.createCopy().release());
    overImage.addAndMakeVisible (dp.createCopy().release());

    auto* db = new DrawableButton ("tabs", DrawableButton::ImageFitted);
    db->setImages (&normalImage, &overImage, nullptr);
    return db;
}

void Windows10LookAndFeel::drawComboBox (Graphics& g, int width, int height, bool isButtonDown,
                                            int, int, int, int, ComboBox& box)
{
    auto boxBounds = Rectangle<int> (width, height).toFloat();

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRect (boxBounds);

    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRect (boxBounds.reduced (0.5f, 0.5f));

    const auto arrowZone = Rectangle<int> (width - 30, 0, 20, height).toFloat();
    constexpr auto arrowSize = float_Pi;

    Path path;
    path.addTriangle ({ arrowZone.getX() + arrowSize, arrowZone.getCentreY() - arrowSize },
                      { arrowZone.getCentreX(), arrowZone.getCentreY() + arrowSize},
                      { arrowZone.getRight() - arrowSize, arrowZone.getCentreY() - float_Pi });

    auto pathColour = box.findColour (ComboBox::arrowColourId);
    if (! box.isEnabled())
        pathColour = pathColour.withAlpha (0.2f);
    else if (isButtonDown)
        pathColour = pathColour.withAlpha (0.1f);

    g.setColour (pathColour);
    g.fillPath (path);
}

Font Windows10LookAndFeel::getTextButtonFont (TextButton&, int buttonHeight)
{
    return { jmin (16.0f, (float) buttonHeight * 0.75f) };
}

void Windows10LookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                                                 bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                      .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

    if (shouldDrawButtonAsHighlighted)
        baseColour.brighter (1.0f).brighter();

    g.setColour (baseColour);

    g.fillRect (bounds);
}

void Windows10LookAndFeel::drawToggleButton (Graphics& g, ToggleButton& button,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
{
    const auto fontSize = jmin (16.0f, (float) button.getHeight() * 0.75f);
    const auto tickWidth = fontSize * 1.1f;

    drawTickBox (g, button, 4.0f, ((float) button.getHeight() - tickWidth) * 0.5f,
                 tickWidth, tickWidth,
                 button.getToggleState(),
                 button.isEnabled(),
                 shouldDrawButtonAsHighlighted,
                 shouldDrawButtonAsDown);

    g.setColour (button.findColour (ToggleButton::textColourId));
    g.setFont (fontSize);

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    drawFittedText (g, button.getButtonText(),
                    button.getLocalBounds().toFloat()
                        .withTrimmedLeft (tickWidth + 10.0f)
                        .withTrimmedRight (2.0f),
                    Justification::centredLeft, 10);
}

int Windows10LookAndFeel::getTextButtonWidthToFitText (TextButton& b, int buttonHeight)
{
    return getTextButtonFont (b, buttonHeight).getStringWidth (b.getButtonText()) + buttonHeight;
}

void Windows10LookAndFeel::drawDrawableButton (Graphics& g, DrawableButton& button, bool, bool)
{
    const bool toggleState = button.getToggleState();

    g.fillAll (button.findColour (toggleState ? DrawableButton::backgroundOnColourId : DrawableButton::backgroundColourId));

    const auto textH = button.getStyle() == DrawableButton::ImageAboveTextLabel
                        ? jmin (16.0f, (float) button.getHeight() * 0.25f)
                        : 0.0f;

    if (textH > 0.0f)
    {
        g.setFont (textH);

        g.setColour (button.findColour (toggleState ? DrawableButton::textColourOnId : DrawableButton::textColourId)
                           .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.4f));

        drawFittedText (g, button.getButtonText(),
                        Rectangle<float> (2.0f, (float) button.getHeight() - textH - 1.0f, (float) button.getWidth() - 4.0f, textH),
                        Justification::centred);
    }
}

void Windows10LookAndFeel::changeToggleButtonWidthToFitText (ToggleButton& button)
{
    const auto fontSize = jmin (15.0f, (float) button.getHeight() * 0.75f);
    const auto tickWidth = fontSize * 1.1f;

    button.setSize (roundToIntAccurate (Font (fontSize).getStringWidthFloat (button.getButtonText()) + tickWidth + 14.0f),
                    button.getHeight());
}

void Windows10LookAndFeel::drawButtonText (Graphics& g, TextButton& button, bool, bool)
{
    const auto font = getTextButtonFont (button, button.getHeight());
    g.setFont (font);
    g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
                                                            : TextButton::textColourOffId)
                       .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    const auto yIndent      = jmin (4.0f, (float) button.getHeight() * 0.3f);
    const auto textWidth    = (float) button.getWidth();

    if (textWidth > 0.0f)
        drawFittedText (g, button.getButtonText(),
                        Rectangle<float> (0.0f, yIndent, textWidth, (float) button.getHeight() - yIndent * 2.0f),
                        Justification::centred, 2, 1.0f);
}

void Windows10LookAndFeel::drawTickBox (Graphics& g, Component& component,
                                        float x, float y, float w, float h,
                                        const bool ticked,
                                        const bool isEnabled,
                                        const bool shouldDrawButtonAsHighlighted,
                                        const bool shouldDrawButtonAsDown)
{
    ignoreUnused (isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    const auto tickBounds = Rectangle<float> (x, y, w, h);

    g.setColour (component.findColour (ToggleButton::tickDisabledColourId));
    g.drawRect (tickBounds);

    if (ticked)
    {
        g.setColour (component.findColour (ToggleButton::tickColourId));
        const auto tick = getTickShape (0.75f);
        g.fillPath (tick, tick.getTransformToScaleToFit (tickBounds.reduced (4.0f, 5.0f), false));
    }
}

void Windows10LookAndFeel::fillTextEditorBackground (Graphics& g, int width, int height, TextEditor& textEditor)
{
    if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) != nullptr)
    {
        g.setColour (textEditor.findColour (TextEditor::backgroundColourId));
        g.fillRect (0, 0, width, height);

        g.setColour (textEditor.findColour (TextEditor::outlineColourId));
        g.drawHorizontalLine (height - 1, 0.0f, static_cast<float> (width));
    }
    else
    {
        g.setColour (textEditor.findColour (TextEditor::backgroundColourId));
        g.fillRect (Rectangle<int> (width, height).toFloat());
    }
}

void Windows10LookAndFeel::drawTextEditorOutline (Graphics& g, int width, int height, TextEditor& textEditor)
{
    if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) == nullptr)
    {
        if (textEditor.isEnabled())
        {
            if (textEditor.hasKeyboardFocus (true) && ! textEditor.isReadOnly())
                g.setColour (textEditor.findColour (TextEditor::focusedOutlineColourId));
            else
                g.setColour (textEditor.findColour (TextEditor::outlineColourId));

            g.drawRect (Rectangle<int> (width, height).toFloat());
        }
    }
}

void Windows10LookAndFeel::paintToolbarBackground (Graphics& g, int, int, Toolbar& toolbar)
{
    g.setColour (toolbar.findColour (Toolbar::backgroundColourId));
    g.fillRect (toolbar.getBounds());
}

//==============================================================================
void Windows10LookAndFeel::drawScrollbar (Graphics& g, ScrollBar& scrollbar,
                                          int x, int y, int width, int height,
                                          bool isScrollbarVertical, int thumbStartPosition,
                                          int thumbSize, bool isMouseOver, bool isMouseDown)
{
    g.fillAll (scrollbar.findColour (ScrollBar::backgroundColourId));

    Rectangle<int> thumbBounds;

    if (isScrollbarVertical)
        thumbBounds = { x, thumbStartPosition, width, thumbSize };
    else
        thumbBounds = { thumbStartPosition, y, thumbSize, height };

    auto c = scrollbar.findColour (ScrollBar::thumbColourId);

    if (isMouseOver || isMouseDown)
        c = c.brighter (0.5f);

    if (isMouseOver && isMouseDown)
        c = c.darker (0.05f);

    g.setColour (c);
    g.fillRect (thumbBounds.reduced (2).toFloat());
}

//==============================================================================
void Windows10LookAndFeel::drawPopupMenuBackground (Graphics& g, int width, int height)
{
    g.fillAll (findColour (PopupMenu::backgroundColourId));
    g.setColour (Colours::black.withAlpha (0.6f));
    g.drawRect (0, 0, width, height);
}

void Windows10LookAndFeel::drawMenuBarBackground (Graphics& g, int /*width*/, int /*height*/, bool, MenuBarComponent& menuBar)
{
    g.fillAll (menuBar.findColour (PopupMenu::backgroundColourId));
}
