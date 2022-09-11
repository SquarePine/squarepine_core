DemoLookAndFeel::DemoLookAndFeel (SharedObjects& sharedObjs) :
    sharedObjects (sharedObjs)
{
    Font::setDefaultMinimumHorizontalScaleFactor (1.0f);

    jassert (sharedObjects.defaultFamily != nullptr);
    setDefaultSansSerifTypefaceName (sharedObjects.defaultFamily->name);

    const auto darkest = colours::darkest;

    setColourScheme
    ({
        darkest,        darkest,                darkest,
        darkest,        Colours::white,         darkest,
        Colours::grey,  Colours::lightgrey,     Colours::white
    });

    setColour (CaretComponent::caretColourId,       Colours::white);
    setColour (Slider::textBoxOutlineColourId,      Colours::transparentBlack);
    setColour (TextButton::buttonColourId,          Colours::white);
    setColour (TextButton::buttonOnColourId,        colours::auth::pinkish);
    setColour (TextButton::textColourOffId,         darkest);
    setColour (TextButton::textColourOnId,          Colours::white);
    setColour (TextEditor::backgroundColourId,      Colours::transparentBlack);
    setColour (TextEditor::focusedOutlineColourId,  Colours::transparentBlack);
    setColour (TextEditor::highlightColourId,       colours::auth::pinkish);
    setColour (TextEditor::highlightedTextColourId, Colours::white);
    setColour (TextEditor::outlineColourId,         Colours::transparentBlack);
    setColour (TextEditor::shadowColourId,          Colours::transparentBlack);
    setColour (TooltipWindow::backgroundColourId,   darkest);
    setColour (TooltipWindow::outlineColourId,      darkest);
    setColour (TooltipWindow::textColourId,         Colours::white);
}

DemoLookAndFeel::~DemoLookAndFeel()
{
}

//==============================================================================
Typeface::Ptr DemoLookAndFeel::getTypefaceForFont (const Font& f)
{
    Typeface::Ptr typefaceToUse;

    if (f.getTypefaceName() == Font::getDefaultSansSerifFontName()
        || f.getTypefaceName() == Font::getDefaultSerifFontName()
        || f.getTypefaceName() == Font::getDefaultMonospacedFontName())
    {
        Font fo (f);
        fo.setTypefaceName (sharedObjects.defaultFamily->name);

        typefaceToUse = getTypefaceFromFamily (fo, *sharedObjects.defaultFamily);
    }

    if (typefaceToUse == nullptr) typefaceToUse = getTypefaceFromFamily (f, *sharedObjects.defaultFamily);
    if (typefaceToUse == nullptr) typefaceToUse = getTypefaceFromFamily (f, sharedObjects.lato);

    if (typefaceToUse != nullptr)
        return typefaceToUse;

    return LookAndFeel_V4::getTypefaceForFont (f);
}

//==============================================================================
TextLayout DemoLookAndFeel::layoutTooltipText (const String& text, Colour textColour) const
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

Rectangle<float> DemoLookAndFeel::getTooltipBoundsFloat (const String& tipText, Point<int> screenPos, Rectangle<int> parentArea) const
{
    const auto tl = layoutTooltipText (tipText, Colours::black);

    return Rectangle<float> (tl.getWidth(), tl.getHeight())
            .withPosition (screenPos.toFloat())
            .constrainedWithin (parentArea.toFloat());
}

Rectangle<int> DemoLookAndFeel::getTooltipBounds (const String& tipText, Point<int> screenPos, Rectangle<int> parentArea)
{
    return getTooltipBoundsFloat (tipText, screenPos, parentArea)
            .getSmallestIntegerContainer();
}

void DemoLookAndFeel::drawTooltip (Graphics& g, const String& text, int w, int h)
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
        g.drawRect (bounds, 1.5f);
    }
}

void DemoLookAndFeel::drawMenuBarBackground (Graphics& g, int width, int height,
                                             bool, MenuBarComponent&)
{
    g.setColour (colours::darkest);
    g.fillRect (Rectangle<int> (width, height));
}

void DemoLookAndFeel::drawMenuBarItem (Graphics& g, int width, int height,
                                       int itemIndex, const String& itemText,
                                       bool isMouseOverItem, bool isMenuOpen,
                                       bool /*isMouseOverBar*/, MenuBarComponent& menuBar)
{
    if (! menuBar.isEnabled())
    {
        g.setColour (colours::darkest.brighter().withMultipliedAlpha (0.5f));
    }
    else if (isMenuOpen || isMouseOverItem)
    {
        g.fillAll (colours::darkest.brighter());
        g.setColour (Colours::white);
    }
    else
    {
        g.setColour (Colours::white);
    }

    g.setFont (getMenuBarFont (menuBar, itemIndex, itemText));
    g.drawFittedText (itemText, 0, 0, width, height, Justification::centred, 1);
}

//==============================================================================
int DemoLookAndFeel::getTabButtonBestWidth (TabBarButton& button, int tabDepth)
{
    auto width = Font ((float) tabDepth).getStringWidth (button.getButtonText().trim())
                    + getTabButtonOverlap (tabDepth) * 2;

    if (auto* extraComponent = button.getExtraComponent())
        width += button.getTabbedButtonBar().isVertical() ? extraComponent->getHeight()
                                                          : extraComponent->getWidth();

    return jlimit (tabDepth * 2, tabDepth * 8, width);
}

void DemoLookAndFeel::drawTextInRegion (Graphics& g, const Font& font, const String& text, Justification j,
                                        const Rectangle<int>& region, Colour colour, bool isEnabled)
{
    g.setColour (colour.withMultipliedAlpha (isEnabled ? 1.0f : 0.5f));
    g.setFont (font);

    drawFittedText (g, text, region, j, roundToIntAccurate ((double) region.getHeight() / (double) font.getHeight()));
}

Grid DemoLookAndFeel::createDefaultGrid()
{
    Grid grid;
    grid.autoRows = grid.autoColumns = Grid::TrackInfo (1_fr);
    return grid;
}

void DemoLookAndFeel::drawLabel (Graphics& g, Label& label)
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

Rectangle<int> DemoLookAndFeel::getTabButtonExtraComponentBounds (const TabBarButton& button, Rectangle<int>& textArea, Component& comp)
{
    Rectangle<int> extraComp;

    const auto orientation = button.getTabbedButtonBar().getOrientation();

    if (button.getExtraComponentPlacement() == TabBarButton::beforeText)
    {
        switch (orientation)
        {
            case TabbedButtonBar::TabsAtBottom:
            case TabbedButtonBar::TabsAtTop:     extraComp = textArea.removeFromLeft   (comp.getWidth()); break;
            case TabbedButtonBar::TabsAtLeft:    extraComp = textArea.removeFromBottom (comp.getHeight()); break;
            case TabbedButtonBar::TabsAtRight:   extraComp = textArea.removeFromTop    (comp.getHeight()); break;
            default:                             jassertfalse; break;
        }
    }
    else
    {
        switch (orientation)
        {
            case TabbedButtonBar::TabsAtBottom:
            case TabbedButtonBar::TabsAtTop:     extraComp = textArea.removeFromRight  (comp.getWidth()); break;
            case TabbedButtonBar::TabsAtLeft:    extraComp = textArea.removeFromTop    (comp.getHeight()); break;
            case TabbedButtonBar::TabsAtRight:   extraComp = textArea.removeFromBottom (comp.getHeight()); break;
            default:                             jassertfalse; break;
        }
    }

    return extraComp;
}

void DemoLookAndFeel::createTabButtonShape (TabBarButton& button, Path& p, bool, bool)
{
    p.addRectangle (button.getLocalBounds().toFloat());
}

void DemoLookAndFeel::fillTabButtonShape (TabBarButton& button, Graphics& g, const Path& path, bool, bool)
{
    const auto tabBackground = button.getTabBackgroundColour();
    const bool isFrontTab = button.isFrontTab();

    if (isFrontTab)
    {
        g.setColour (Colours::green);
        g.fillPath (path);
    }

    g.setColour (isFrontTab ? tabBackground : tabBackground.withMultipliedAlpha (0.9f));
    g.fillPath (path);
}

void DemoLookAndFeel::drawTabButtonText (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
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

void DemoLookAndFeel::drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
{
    Path tabShape;
    createTabButtonShape (button, tabShape, isMouseOver, isMouseDown);
    fillTabButtonShape (button, g, tabShape, isMouseOver, isMouseDown);
    drawTabButtonText (button, g, isMouseOver, isMouseDown);
}

Button* DemoLookAndFeel::createTabBarExtrasButton()
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

void DemoLookAndFeel::drawComboBox (Graphics& g, int width, int height, bool isButtonDown,
                                    int, int, int, int, ComboBox& box)
{
    const auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : corner;
    const auto boxBounds = Rectangle<int> (width, height).toFloat();

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRoundedRectangle (boxBounds, cornerSize);

    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRoundedRectangle (boxBounds.reduced (0.5f, 0.5f), cornerSize, 1.0f);

    const auto arrowZone = Rectangle<int> (width - 30, 0, 20, height).toFloat();
    constexpr auto arrowSize = MathConstants<float>::pi;

    Path path;
    path.addTriangle ({ arrowZone.getX() + arrowSize, arrowZone.getCentreY() - arrowSize },
                      { arrowZone.getCentreX(), arrowZone.getCentreY() + arrowSize},
                      { arrowZone.getRight() - arrowSize, arrowZone.getCentreY() - MathConstants<float>::pi });

    auto pathColour = box.findColour (ComboBox::arrowColourId);
    if (! box.isEnabled())
        pathColour = pathColour.withAlpha (0.2f);
    else if (isButtonDown)
        pathColour = pathColour.withAlpha (0.1f);

    g.setColour (pathColour);
    g.fillPath (path);
}

Font DemoLookAndFeel::getTextButtonFont (TextButton&, int)
{
    return { defaultFontHeight };
}

void DemoLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto cornerSize = corner;
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                      .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

    if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter (1.0f).brighter();

    g.setColour (baseColour);

    const auto flatOnLeft   = button.isConnectedOnLeft();
    const auto flatOnRight  = button.isConnectedOnRight();
    const auto flatOnTop    = button.isConnectedOnTop();
    const auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
    {
        Path path;
        path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                  bounds.getWidth(), bounds.getHeight(),
                                  cornerSize, cornerSize,
                                  ! (flatOnLeft || flatOnTop),
                                  ! (flatOnRight || flatOnTop),
                                  ! (flatOnLeft || flatOnBottom),
                                  ! (flatOnRight || flatOnBottom));

        g.fillPath (path);
    }
    else
    {
        g.fillRoundedRectangle (bounds, cornerSize);
    }
}

void DemoLookAndFeel::drawToggleButton (Graphics& g, ToggleButton& button,
                                        bool shouldDrawButtonAsHighlighted,
                                        bool shouldDrawButtonAsDown)
{
    const auto fontSize = defaultFontHeight;
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

int DemoLookAndFeel::getTextButtonWidthToFitText (TextButton& b, int buttonHeight)
{
    return getTextButtonFont (b, buttonHeight).getStringWidth (b.getButtonText()) + buttonHeight;
}

void DemoLookAndFeel::drawDrawableButton (Graphics& g, DrawableButton& button, bool, bool)
{
    const bool toggleState = button.getToggleState();

    g.fillAll (button.findColour (toggleState ? DrawableButton::backgroundOnColourId : DrawableButton::backgroundColourId));

    const auto textH = button.getStyle() == DrawableButton::ImageAboveTextLabel
                        ? defaultFontHeight
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

void DemoLookAndFeel::changeToggleButtonWidthToFitText (ToggleButton& button)
{
    const auto fontSize = defaultFontHeight;
    const auto tickWidth = fontSize * 1.1f;

    button.setSize (roundToIntAccurate (Font (fontSize).getStringWidthFloat (button.getButtonText()) + tickWidth + defaultFontHeight),
                    button.getHeight());
}

void DemoLookAndFeel::drawButtonText (Graphics& g, TextButton& button, bool, bool)
{
    const auto font = getTextButtonFont (button, button.getHeight());
    g.setFont (font);
    g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
                                                            : TextButton::textColourOffId)
                       .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    const auto yIndent      = jmin (4.0f, (float) button.getHeight() * 0.3f);
    const auto leftIndent   = jmin (defaultFontHeight, 2.0f + corner / (button.isConnectedOnLeft() ? 4.0f : 2.0f));
    const auto rightIndent  = jmin (defaultFontHeight, 2.0f + corner / (button.isConnectedOnRight() ? 4.0f : 2.0f));
    const auto textWidth    = (float) button.getWidth() - leftIndent - rightIndent;

    if (textWidth > 0.0f)
        drawFittedText (g, button.getButtonText(),
                        Rectangle<float> (leftIndent, yIndent, textWidth, (float) button.getHeight() - yIndent * 2.0f),
                        Justification::centred, 2, 1.0f);
}

void DemoLookAndFeel::drawTickBox (Graphics& g, Component& component,
                                   float x, float y, float w, float h,
                                   const bool ticked,
                                   const bool isEnabled,
                                   const bool shouldDrawButtonAsHighlighted,
                                   const bool shouldDrawButtonAsDown)
{
    ignoreUnused (isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    const auto tickBounds = Rectangle<float> (x, y, w, h);

    g.setColour (component.findColour (ToggleButton::tickDisabledColourId));
    g.drawRoundedRectangle (tickBounds, 4.0f, 1.0f);

    if (ticked)
    {
        g.setColour (component.findColour (ToggleButton::tickColourId));
        const auto tick = getTickShape (0.75f);
        g.fillPath (tick, tick.getTransformToScaleToFit (tickBounds.reduced (4.0f, 5.0f), false));
    }
}

void DemoLookAndFeel::fillTextEditorBackground (Graphics& g, int width, int height, TextEditor& textEditor)
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
        g.fillRoundedRectangle (Rectangle<int> (width, height).toFloat(), corner);
    }
}

void DemoLookAndFeel::drawTextEditorOutline (Graphics& g, int width, int height, TextEditor& textEditor)
{
    if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) == nullptr)
    {
        if (textEditor.isEnabled())
        {
            if (textEditor.hasKeyboardFocus (true) && ! textEditor.isReadOnly())
                g.setColour (textEditor.findColour (TextEditor::focusedOutlineColourId));
            else
                g.setColour (textEditor.findColour (TextEditor::outlineColourId));

            g.drawRoundedRectangle (Rectangle<int> (width, height).toFloat(), corner, 1.0f);
        }
    }
}

void DemoLookAndFeel::paintToolbarBackground (Graphics& g, int, int, Toolbar& toolbar)
{
    g.setColour (toolbar.findColour (Toolbar::backgroundColourId));
    g.fillRect (toolbar.getBounds());
}

//==============================================================================
void DemoLookAndFeel::drawScrollbar (Graphics& g, ScrollBar& scrollbar,
                                     int x, int y, int width, int height,
                                     bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
                                     bool isMouseOver, bool isMouseDown)
{
    g.fillAll (scrollbar.findColour (ScrollBar::backgroundColourId));

    Rectangle<int> thumbBounds;

    if (isScrollbarVertical)
        thumbBounds = { x, thumbStartPosition, width, thumbSize };
    else
        thumbBounds = { thumbStartPosition, y, thumbSize, height };

    auto c = scrollbar.findColour (ScrollBar::thumbColourId);
    if (isMouseOver || isMouseDown) c = c.brighter (0.5f);
    if (isMouseOver && isMouseDown) c = c.darker (0.05f);

    g.setColour (c);
    g.fillRoundedRectangle (thumbBounds.reduced (2).toFloat(), MathConstants<float>::pi);
}

//==============================================================================
void DemoLookAndFeel::drawPopupMenuBackground (Graphics& g, int width, int height)
{
    g.fillAll (findColour (PopupMenu::backgroundColourId));
    g.setColour (Colours::black.withAlpha (0.6f));
    g.drawRect (0, 0, width, height);
}

//==============================================================================
void DemoLookAndFeel::drawStretchableLayoutResizerBar (Graphics& g, int /*w*/, int /*h*/, bool /*isVerticalBar*/,
                                                       bool isMouseOver, bool isMouseDragging)
{
    if (isMouseOver || isMouseDragging)
        g.fillAll (Colours::darkgrey.withAlpha (0.5f));
    else
        g.fillAll (Colours::grey);
}
