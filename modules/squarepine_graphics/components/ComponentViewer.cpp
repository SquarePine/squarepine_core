inline Component* realGetComponent (Component& p, Point<int> screenPos)
{
    if (p.getScreenBounds().contains (screenPos))
    {
        for (auto* c : p.getChildren())
            if (auto* r = realGetComponent (*c, screenPos))
                return r;

        return &p;
    }

    return nullptr;
}

inline Component* realGetComponentUnderMouse()
{
    const auto mouse = Desktop::getInstance().getMainMouseSource();
    const auto pos = mouse.getScreenPosition().toInt();

    auto& desktop = Desktop::getInstance();

    for (int i = desktop.getNumComponents(); --i >= 0;)
        if (auto* dtc = desktop.getComponent (i))
            if (dtc->isVisible())
                if (auto* c = realGetComponent (*dtc, pos))
                    return c;

    return {};
}

//==============================================================================
class ComponentViewer::Snapshot final : public Component
{
public:
    Snapshot() = default;
    ~Snapshot() override = default;

    void update (Component* c, Point<int> pos, int zoom_)
    {
        zoom = zoom_;

        if (c == nullptr)
        {
            clear();
            return;
        }

        const auto w = getWidth() / zoom + 1;
        const auto h = getHeight() / zoom + 1;
        const auto root = c->getTopLevelComponent();
        const auto rootPos = root->getLocalPoint (c, pos);

        if (auto* disp = Desktop::getInstance().getDisplays().getDisplayForPoint (c->localPointToGlobal (pos)))
            scale = (float) disp->scale;

        image = root->createComponentSnapshot ({ rootPos.getX() - w / 2, rootPos.getY() - h / 2, w, h }, false, scale);
        image = image.rescaled (w * zoom, h * zoom);

        repaint();
    }

    void clear()
    {
        image = {};
        repaint();
    }

    Colour getColourUnderMouse() const
    {
        if (image.isValid() && zoom > 0)
        {
            const auto w = getWidth() / zoom + 1;
            const auto h = getHeight() / zoom + 1;
            const auto x = w / 2 * zoom;
            const auto y = h / 2 * zoom;

            return image.getPixelAt (x, y);
        }

        return {};
    }

    void paint (Graphics& g) override
    {
        if (image.isValid() && zoom > 0 && scale != 0.0f)
        {
            g.drawImageAt (image, 0, 0);

            const auto w = getWidth() / zoom + 1;
            const auto h = getHeight() / zoom + 1;
            const auto z = roundToInt ((float) zoom / scale);
            const auto rc = Rectangle<int> (w / 2 * zoom, h / 2 * zoom, z, z);
            const auto c = image.getPixelAt (rc.getX(), rc.getY());

            g.setColour (c.contrasting());
            g.drawRect (rc, 1);
        }
    }

private:
    Image image;
    int zoom = 10;
    float scale = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Snapshot)
};

//==============================================================================
class ComponentViewer::ContentComponent final : public Component,
                                                private Timer,
                                                private Slider::Listener
{
public:
    ContentComponent()
    {
        const auto f = Font (Font::getDefaultMonospacedFontName(), 20.0f, Font::plain);

        details.setFont (f);
        details.setMultiLine (true, false);
        details.setReadOnly (true);

        snapshotDetails.setFont (f);
        snapshotDetails.setReadOnly (true);
        snapshotDetails.setJustification (Justification::centred);

        zoom.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        zoom.setRange (1, 30, 1);
        zoom.setValue (10);
        zoom.addListener (this);

        addAndMakeVisible (details);
        addAndMakeVisible (snapshotDetails);
        addAndMakeVisible (zoom);
        addAndMakeVisible (snapshot);

        Desktop::getInstance().addGlobalMouseListener (this);
    }

    ~ContentComponent() override
    {
        Desktop::getInstance().removeGlobalMouseListener (this);
    }

    void updateComponentDetailsAsync()
    {
        if (! isTimerRunning())
            startTimer (30);
    }

    void updateComponentDetails()
    {
        const auto mouse = Desktop::getInstance().getMainMouseSource();
        const auto pos = mouse.getScreenPosition().toInt();

        StringArray componentHierarchy;
        String cursorPos, colourDetails;

        if (auto* c = realGetComponentUnderMouse())
        {
            snapshot.update (c, c->getLocalPoint (nullptr, pos), roundToInt (zoom.getValue()));
            componentHierarchy = getComponentHierarchy (c);

            cursorPos
                << "Component: (" << c->getLocalPoint (nullptr, pos).toString() << ")" << newLine
                << "Window: (" << c->getTopLevelComponent()->getLocalPoint (nullptr, pos).toString() << ")" << newLine;

            colourDetails = snapshot.getColourUnderMouse().toDisplayString (true);
        }
        else
        {
            snapshot.clear();
        }

        cursorPos << "Screen: (" + pos.toString() + ")" << newLine << "--------------------" << newLine;

        details.setText (cursorPos + componentHierarchy.joinIntoString (newLine));
        snapshotDetails.setText (colourDetails);
    }

    static void appendBool (String& dest, const String& name, bool state, bool alwaysDisplay = false)
    {
        if (state)
            dest << spacer << name << newLine;

        if (alwaysDisplay && ! state)
            dest << spacer << "Not " << name << newLine;
    }

    static StringArray getComponentHierarchy (Component* c)
    {
        StringArray res;
        String str;

        for (; c != nullptr; c = c->getParentComponent())
        {
            str << "[" << getDemangledName (c) << "]" << newLine;

            const auto n = c->getName();
            if (n.isNotEmpty())
                str << spacer << "Name: " << c->getName() << newLine;

            const auto b = c->getBounds();
            if (! b.isEmpty())
                str << spacer << "(" << b.toString() << ")" << newLine;

            if (auto* ttc = dynamic_cast<TooltipClient*> (c))
            {
                const auto s = ttc->getTooltip();
                if (s.isNotEmpty())
                    str << spacer << "Tooltip: " << newLine << s << newLine;
            }

            appendBool (str, "Opaque", c->isOpaque(), true);
            appendBool (str, "Enabled", c->isEnabled(), true);
            appendBool (str, "Visible", c->isVisible(), true);
            appendBool (str, "Showing", c->isShowing(), true);
            appendBool (str, "Unclipped", c->isPaintingUnclipped(), true);
            appendBool (str, "On Desktop", c->isOnDesktop());
            appendBool (str, "Always On Top", c->isAlwaysOnTop());
            appendBool (str, "Transformed", c->isTransformed());
            appendBool (str, "Brought to Front On Mouse Click", c->isBroughtToFrontOnMouseClick());
            appendBool (str, "Focus Container", c->isFocusContainer());

            res.add (str);
            str.clear();
        }

        return res;
    }

    void resized() override
    {
        auto rc = getLocalBounds();

        details.setBounds (rc.removeFromTop (roundToInt ((double) rc.getHeight() * 0.75)));

        auto row = rc.removeFromTop (25);
        const auto w = row.getWidth() / 3;
        zoom.setBounds (row.removeFromLeft (w));
        snapshotDetails.setBounds (row.removeFromLeft (w));

        snapshot.setBounds (rc);
    }
    
    void mouseUp (const MouseEvent&) override   { updateComponentDetailsAsync(); }
    void mouseDown (const MouseEvent&) override { updateComponentDetailsAsync(); }
    void mouseDrag (const MouseEvent&) override { updateComponentDetailsAsync(); }
    void mouseMove (const MouseEvent&) override { updateComponentDetailsAsync(); }
    void timerCallback() override               { stopTimer(); updateComponentDetails(); }
    void sliderValueChanged (Slider*) override  { updateComponentDetails(); }

    inline static const char* const spacer = "    ";
    TextEditor details, snapshotDetails;
    Slider zoom;
    Snapshot snapshot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ContentComponent)
};

//==============================================================================
ComponentViewer::ComponentViewer() :
    Component ("Component Viewer")
{
    setOpaque (true);

    contentComponent.reset (new ContentComponent());
    addAndMakeVisible (contentComponent.get());

    setSize (640, 480);
}

void ComponentViewer::lookAndFeelChanged()
{
    repaint();
}

void ComponentViewer::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void ComponentViewer::resized()
{
    if (contentComponent != nullptr)
        contentComponent->setBounds (getLocalBounds());
}
