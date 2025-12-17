//==============================================================================
inline String getFormatSuffix (const AudioProcessor* plugin)
{
    const auto format = [plugin]()
    {
        if (auto* instance = dynamic_cast<const AudioPluginInstance*> (plugin))
            return instance->getPluginDescription().pluginFormatName;

        return String();
    }();

    return format.isNotEmpty() ? (" (" + format + ")") : format;
}

//==============================================================================
/**
    A desktop window containing a plugin's GUI.
*/
class PluginWindow final : public DocumentWindow
{
public:
    enum class Type
    {
        normal = 0,
        generic,
        programs
    };

    PluginWindow (AudioProcessorGraph::Node* n, Type t, OwnedArray<PluginWindow>& windowList) :
        DocumentWindow (n->getProcessor()->getName() + getFormatSuffix (n->getProcessor()),
                        LookAndFeel::getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                        DocumentWindow::minimiseButton | DocumentWindow::closeButton),
        activeWindowList (windowList),
        node (n),
        type (t)
    {
        setSize (400, 300);

        if (auto* ui = createProcessorEditor (*node->getProcessor(), type))
        {
            setContentOwned (ui, true);
            setResizable (ui->isResizable(), false);
        }

        setConstrainer (&constrainer);

       #if SQUAREPINE_IS_MOBILE
        const auto screenBounds = Desktop::getInstance().getDisplays().getTotalBounds (true).toFloat();
        const auto scaleFactor = jmin ((screenBounds.getWidth()  - 50.0f) / (float) getWidth(),
                                       (screenBounds.getHeight() - 50.0f) / (float) getHeight());

        if (scaleFactor < 1.0f)
        {
            setSize ((int) (scaleFactor * (float) getWidth()),
                     (int) (scaleFactor * (float) getHeight()));
        }

        setTopLeftPosition (20, 20);
       #else
        setTopLeftPosition (node->properties.getWithDefault (getLastXProp (type), Random::getSystemRandom().nextInt (500)),
                            node->properties.getWithDefault (getLastYProp (type), Random::getSystemRandom().nextInt (500)));
       #endif

        node->properties.set (getOpenProp (type), true);

        setVisible (true);
    }

    ~PluginWindow() override
    {
        clearContentComponent();
    }

    void moved() override
    {
        node->properties.set (getLastXProp (type), getX());
        node->properties.set (getLastYProp (type), getY());
    }

    void closeButtonPressed() override
    {
        node->properties.set (getOpenProp (type), false);
        activeWindowList.removeObject (this);
    }

    static String getLastXProp (Type type)    { return "uiLastX_" + getTypeName (type); }
    static String getLastYProp (Type type)    { return "uiLastY_" + getTypeName (type); }
    static String getOpenProp  (Type type)    { return "uiopen_"  + getTypeName (type); }

    OwnedArray<PluginWindow>& activeWindowList;
    const AudioProcessorGraph::Node::Ptr node;
    const Type type;

    BorderSize<int> getBorderThickness() const override
    {
       #if JUCE_IOS || JUCE_ANDROID
        const int border = 10;
        return { border, border, border, border };
       #else
        return DocumentWindow::getBorderThickness();
       #endif
    }

private:
    class DecoratorConstrainer final : public BorderedComponentBoundsConstrainer
    {
    public:
        explicit DecoratorConstrainer (DocumentWindow& windowIn) :
            window (windowIn)
        {
        }

        ComponentBoundsConstrainer* getWrappedConstrainer() const override
        {
            auto* editor = dynamic_cast<AudioProcessorEditor*> (window.getContentComponent());
            return editor != nullptr ? editor->getConstrainer() : nullptr;
        }

        BorderSize<int> getAdditionalBorder() const override
        {
            const auto nativeFrame = [&]() -> BorderSize<int>
            {
                if (auto* peer = window.getPeer())
                    if (const auto frameSize = peer->getFrameSizeIfPresent())
                        return *frameSize;

                return {};
            }();

            return nativeFrame.addedTo (window.getContentComponentBorder());
        }

    private:
        DocumentWindow& window;
    };

    DecoratorConstrainer constrainer { *this };

    static AudioProcessorEditor* createProcessorEditor (AudioProcessor& processor,
                                                        PluginWindow::Type type)
    {
        if (type == PluginWindow::Type::normal)
        {
            if (processor.hasEditor())
                if (auto* ui = processor.createEditorIfNeeded())
                    return ui;

            type = PluginWindow::Type::generic;
        }

        if (type == PluginWindow::Type::generic)
        {
            auto* result = new GenericAudioProcessorEditor (processor);
            result->setResizeLimits (200, 300, 1000, 10000);
            return result;
        }

        if (type == PluginWindow::Type::programs)
            return new ProgramAudioProcessorEditor (processor);

        jassertfalse;
        return {};
    }

    static String getTypeName (Type type)
    {
        switch (type)
        {
            case Type::normal:     return "Normal";
            case Type::generic:    return "Generic";
            case Type::programs:   return "Programs";
            default:               return {};
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

//==============================================================================
/** */
class AudioProcessorGraphEditor final : public Component,
                                        public AudioProcessorListener,
                                        public ChangeListener,
                                        public Timer
{
public:
    /** */
    AudioProcessorGraphEditor (AudioProcessorGraph&,
                               AudioPluginFormatManager&,
                               KnownPluginList&);
    /** */
    ~AudioProcessorGraphEditor() override;

    //==============================================================================
    using Node              = AudioProcessorGraph::Node;
    using NodeAndChannel    = AudioProcessorGraph::NodeAndChannel;
    using NodeID            = AudioProcessorGraph::NodeID;
    using Connection        = AudioProcessorGraph::Connection;

    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent&) override;
    void mouseDrag (const MouseEvent& e) override;
    void timerCallback() override;
    void resized() override;
    void changeListenerCallback (ChangeBroadcaster*) override;

    void showPopupMenu (Point<int> mousePos)
    {
#if 0
        menu.reset (new PopupMenu());

        if (auto* mainWindow = findParentComponentOfClass<MainHostWindow>())
        {
            mainWindow->addPluginsToMenu (*menu);

            menu->showMenuAsync ({},
                                 ModalCallbackFunction::create ([this, mousePos] (int r)
                                                                {
                                                                    if (auto* mainWin = findParentComponentOfClass<MainHostWindow>())
                                                                        if (const auto chosen = mainWin->getChosenType (r))
                                                                            createNewPlugin (*chosen, mousePos);
                                                                }));
        }
#endif
    }

    /** @internal */
    void audioProcessorParameterChanged (AudioProcessor*, int, float) override {}
    /** @internal */
    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override { changed(); }

private:
    //==============================================================================
    class PinComponent final : public Component,
                               public SettableTooltipClient
    {
    public:
        PinComponent (AudioProcessorGraphEditor& o, AudioProcessorGraph::NodeAndChannel pinToUse, bool isIn) :
            owner (o),
            pin (pinToUse),
            isInput (isIn)
        {
            if (auto node = owner.graph.getNodeForId (pin.nodeID))
            {
                String tip;

                if (pin.isMIDI())
                {
                    tip = isInput ? "MIDI Input"
                                  : "MIDI Output";
                }
                else
                {
                    auto& processor = *node->getProcessor();
                    auto channel = processor.getOffsetInBusBufferForAbsoluteChannelIndex (isInput, pin.channelIndex, busIdx);

                    if (auto* bus = processor.getBus (isInput, busIdx))
                        tip = bus->getName() + ": " + AudioChannelSet::getAbbreviatedChannelTypeName (bus->getCurrentLayout().getTypeOfChannel (channel));
                    else
                        tip = (isInput ? "Main Input: " : "Main Output: ") + String (pin.channelIndex + 1);
                }

                setTooltip (tip);
            }

            setSize (16, 16);
        }

        void paint (Graphics& g) override
        {
            auto w = (float) getWidth();
            auto h = (float) getHeight();

            Path p;
            p.addEllipse (w * 0.25f, h * 0.25f, w * 0.5f, h * 0.5f);
            p.addRectangle (w * 0.4f, isInput ? (0.5f * h) : 0.0f, w * 0.2f, h * 0.5f);

            auto colour = (pin.isMIDI() ? Colours::red : Colours::green);

            g.setColour (colour.withRotatedHue ((float) busIdx / 5.0f));
            g.fillPath (p);
        }

        void mouseDown (const MouseEvent& e) override
        {
            AudioProcessorGraph::NodeAndChannel dummy;

            owner.beginConnectorDrag (isInput ? dummy : pin,
                                      isInput ? pin : dummy,
                                      e);
        }

        void mouseDrag (const MouseEvent& e) override
        {
            owner.dragConnector (e);
        }

        void mouseUp (const MouseEvent& e) override
        {
            owner.endDraggingConnector (e);
        }

        AudioProcessorGraphEditor& owner;
        NodeAndChannel pin;
        const bool isInput;
        int busIdx = 0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PinComponent)
    };

    //==============================================================================
    /** */
    class NodeView final : public Component,
                           public Timer,
                           public AsyncUpdater,
                           public SettableTooltipClient,
                           public AudioProcessorListener,
                           public AudioProcessorParameter::Listener
    {
    public:
        /** */
        explicit NodeView (AudioProcessorGraphEditor&, Node::Ptr);
        /** */
        ~NodeView() override;

        void handleAsyncUpdate() override { repaint(); }

        void timerCallback() override
        {
            // this should only be called on touch devices
            jassert (isOnTouchDevice());

            stopTimer();
            // showPopupMenu();
        }

        void update()
        {
            const auto pluginID = node->nodeID;
            auto processor = getProcessor();
            const auto formatSuffix = getFormatSuffix (processor);

            numIns = processor->getTotalNumInputChannels();
            if (processor->acceptsMidi())
                ++numIns;

            numOuts = processor->getTotalNumOutputChannels();
            if (processor->producesMidi())
                ++numOuts;

            int w = 100;
            int h = 60;

            w = jmax (w, (jmax (numIns, numOuts) + 1) * 20);

            const auto textWidth = GlyphArrangement::getStringWidthInt (font, processor->getName());
            w = jmax (w, 16 + jmin (textWidth, 300));
            if (textWidth > 300)
                h = 100;

            setSize (w, h);
            setName (processor->getName() + formatSuffix);

            {
                auto p = owner.getNodePosition (pluginID);
                setCentreRelative ((float) p.x, (float) p.y);
            }

            if (numIns != numInputs || numOuts != numOutputs)
            {
                numInputs = numIns;
                numOutputs = numOuts;

                pins.clear();

                for (int i = 0; i < processor->getTotalNumInputChannels(); ++i)
                    addAndMakeVisible (pins.add (new PinComponent (owner, { pluginID, i }, true)));

                if (processor->acceptsMidi())
                    addAndMakeVisible (pins.add (new PinComponent (owner, { pluginID, AudioProcessorGraph::midiChannelIndex }, true)));

                for (int i = 0; i < processor->getTotalNumOutputChannels(); ++i)
                    addAndMakeVisible (pins.add (new PinComponent (owner, { pluginID, i }, false)));

                if (processor->producesMidi())
                    addAndMakeVisible (pins.add (new PinComponent (owner, { pluginID, AudioProcessorGraph::midiChannelIndex }, false)));

                resized();
            }
        }

        AudioProcessor* getProcessor() const
        {
            if (node != nullptr)
                return node->getProcessor();

            return {};
        }

        Point<float> getPinPos (int index, bool isInput) const
        {
            for (auto* pin : pins)
                if (pin->pin.channelIndex == index && isInput == pin->isInput)
                    return getPosition().toFloat() + pin->getBounds().getCentre().toFloat();

            return {};
        }

        /** @internal */
        void paint (Graphics&) override;
        /** @internal */
        void resized() override;
        /** @internal */
        void mouseDown (const MouseEvent&) override;
        /** @internal */
        void mouseDrag (const MouseEvent&) override;

        void audioProcessorParameterChanged (AudioProcessor*, int, float) override
        {
        }

        void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override
        {
        }

        void parameterValueChanged (int, float) override
        {
        }

        void parameterGestureChanged (int, bool) override
        {
        }

        AudioProcessorGraphEditor& owner;
        Node::Ptr node;

    private:
        Point<int> dragStart;
        OwnedArray<PinComponent> pins;
        int numInputs = 0, numOutputs = 0;
        int numIns = 0, numOuts = 0;
        int pinSize = 16;
        Point<int> originalPos;
        Font font = FontOptions { 13.0f, Font::bold };
        DropShadowEffect shadow;
        std::unique_ptr<PopupMenu> menu;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeView)
    };

    //==============================================================================
    /** */
    class ConnectorComponent final : public Component,
                                     public SettableTooltipClient,
                                     public AudioProcessorListener
    {
    public:
        /** */
        explicit ConnectorComponent (AudioProcessorGraphEditor& o) :
            owner (o)
        {
            setAlwaysOnTop (true);
        }

        ~ConnectorComponent() override
        {
        }

        void audioProcessorParameterChanged (AudioProcessor*, int, float) override
        {
        }

        void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override
        {
        }


        /** */
        void setInput (NodeAndChannel newSource)
        {
            if (connection.source != newSource)
            {
                connection.source = newSource;
                update();
            }
        }

        void setOutput (NodeAndChannel newDest)
        {
            if (connection.destination != newDest)
            {
                connection.destination = newDest;
                update();
            }
        }

        void dragStart (Point<float> pos)
        {
            lastInputPos = pos;
            resizeToFit();
        }

        void dragEnd (Point<float> pos)
        {
            lastOutputPos = pos;
            resizeToFit();
        }

        void update()
        {
            Point<float> p1, p2;
            getPoints (p1, p2);

            if (lastInputPos != p1 || lastOutputPos != p2)
                resizeToFit();
        }

        void resizeToFit()
        {
            Point<float> p1, p2;
            getPoints (p1, p2);

            auto newBounds = Rectangle<float> (p1, p2)
                                .expanded (4.0f)
                                .getSmallestIntegerContainer();

            if (newBounds != getBounds())
                setBounds (newBounds);
            else
                resized();

            repaint();
        }

        void getPoints (Point<float>& p1, Point<float>& p2) const
        {
            p1 = lastInputPos;
            p2 = lastOutputPos;

            if (auto* src = owner.getComponentForPlugin (connection.source.nodeID))
                p1 = src->getPinPos (connection.source.channelIndex, false);

            if (auto* dest = owner.getComponentForPlugin (connection.destination.nodeID))
                p2 = dest->getPinPos (connection.destination.channelIndex, true);
        }

        void paint (Graphics& g) override
        {
            if (connection.source.isMIDI() || connection.destination.isMIDI())
                g.setColour (Colours::red);
            else
                g.setColour (Colours::green);

            g.fillPath (linePath);
        }

        bool hitTest (int x, int y) override
        {
            auto pos = Point<int> (x, y).toFloat();

            if (hitPath.contains (pos))
            {
                double distanceFromStart, distanceFromEnd;
                getDistancesFromEnds (pos, distanceFromStart, distanceFromEnd);

                // avoid clicking the connector when over a pin
                return distanceFromStart > 7.0 && distanceFromEnd > 7.0;
            }

            return false;
        }

        void mouseDown (const MouseEvent&) override
        {
            dragging = false;
        }

        void mouseDrag (const MouseEvent& e) override
        {
            if (dragging)
            {
                owner.dragConnector (e);
            }
            else if (e.mouseWasDraggedSinceMouseDown())
            {
                dragging = true;

                owner.graph.removeConnection (connection);

                double distanceFromStart, distanceFromEnd;
                getDistancesFromEnds (getPosition().toFloat() + e.position, distanceFromStart, distanceFromEnd);
                const bool isNearerSource = distanceFromStart < distanceFromEnd;

                NodeAndChannel dummy { {}, 0 };

                owner.beginConnectorDrag (isNearerSource ? dummy : connection.source,
                                          isNearerSource ? connection.destination : dummy,
                                          e);
            }
        }

        void mouseUp (const MouseEvent& e) override
        {
            if (dragging)
                owner.endDraggingConnector (e);
        }

        void resized() override
        {
            Point<float> p1, p2;
            getPoints (p1, p2);

            lastInputPos = p1;
            lastOutputPos = p2;

            p1 -= getPosition().toFloat();
            p2 -= getPosition().toFloat();

            linePath.clear();
            linePath.startNewSubPath (p1);
            linePath.cubicTo (p1.x, p1.y + (p2.y - p1.y) * 0.33f,
                              p2.x, p1.y + (p2.y - p1.y) * 0.66f,
                              p2.x, p2.y);

            PathStrokeType wideStroke (8.0f);
            wideStroke.createStrokedPath (hitPath, linePath);

            PathStrokeType stroke (2.5f);
            stroke.createStrokedPath (linePath, linePath);

            auto arrowW = 5.0f;
            auto arrowL = 4.0f;

            Path arrow;
            arrow.addTriangle (-arrowL, arrowW,
                               -arrowL, -arrowW,
                               arrowL, 0.0f);

            arrow.applyTransform (AffineTransform()
                                    .rotated (MathConstants<float>::halfPi - (float) std::atan2 (p2.x - p1.x, p2.y - p1.y))
                                    .translated ((p1 + p2) * 0.5f));

            linePath.addPath (arrow);
            linePath.setUsingNonZeroWinding (true);
        }

        void getDistancesFromEnds (Point<float> p, double& distanceFromStart, double& distanceFromEnd) const
        {
            Point<float> p1, p2;
            getPoints (p1, p2);

            distanceFromStart = p1.getDistanceFrom (p);
            distanceFromEnd   = p2.getDistanceFrom (p);
        }

        AudioProcessorGraphEditor& owner;
        Connection connection;
        Point<float> lastInputPos, lastOutputPos;
        Path linePath, hitPath;
        bool dragging = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectorComponent)
    };

    //==============================================================================
    friend NodeView;
    friend ConnectorComponent;
    friend PinComponent;

    AudioProcessorGraph& graph;
    AudioPluginFormatManager& formatManager;
    KnownPluginList& knownPlugins;
    Component house;
    Viewport viewport;
    Point<int> originalTouchPos;
    OwnedArray<NodeView> nodeViews;
    OwnedArray<ConnectorComponent> connectors;
    std::unique_ptr<ConnectorComponent> draggingConnector;
    std::unique_ptr<PopupMenu> menu;

    OwnedArray<PluginWindow> activePluginWindows;
    ScopedMessageBox messageBox;

    NodeID lastUID;

    //==============================================================================
    static bool isOnTouchDevice();
    NodeID getNextUID() noexcept;
    ConnectorComponent* getComponentForConnection (const Connection&) const;
    PinComponent* findPinAt (Point<float>) const;
    Node::Ptr getNodeForName (const String&) const;
    void setNodePosition (NodeID, Point<double>);
    Point<double> getNodePosition (NodeID) const;
    void clear();
    bool closeAnyOpenPluginWindows();
    void rebuild();
    void changed();
    void updateComponents();
    void createNewPlugin (Point<int>);
    NodeView* getComponentForPlugin (NodeID) const;
    void beginConnectorDrag (NodeAndChannel source,  NodeAndChannel dest, const MouseEvent&);
    void dragConnector (const MouseEvent&);
    void endDraggingConnector (const MouseEvent&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessorGraphEditor)
};
