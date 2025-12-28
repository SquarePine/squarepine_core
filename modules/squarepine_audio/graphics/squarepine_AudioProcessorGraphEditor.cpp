//==============================================================================
AudioProcessorGraphEditor::NodeView::NodeView (AudioProcessorGraphEditor& parentOwner,
                                               AudioProcessorGraph::Node::Ptr sourceNode) :
    owner (parentOwner),
    node (sourceNode)
{
    jassert (node != nullptr);

    shadow.setShadowProperties (DropShadow (Colours::black.withAlpha (0.5f), 3, { 0, 1 }));
    setComponentEffect (&shadow);

    if (auto* processor = node->getProcessor())
        if (auto* bypassParam = processor->getBypassParameter())
            bypassParam->addListener (this);
}

AudioProcessorGraphEditor::NodeView::~NodeView()
{
    if (auto* processor = node->getProcessor())
        if (auto* bypassParam = processor->getBypassParameter())
            bypassParam->removeListener (this);
}

void AudioProcessorGraphEditor::NodeView::paint (Graphics& g)
{
    auto boxArea = getLocalBounds().reduced (4, pinSize);
    bool isBypassed = false;

    if (node != nullptr)
        isBypassed = node->isBypassed();

    auto boxColour = findColour (TextEditor::backgroundColourId);

    if (isBypassed)
        boxColour = boxColour.brighter();

    g.setColour (boxColour);
    g.fillRect (boxArea.toFloat());

    g.setColour (findColour (TextEditor::textColourId));
    g.setFont (font);
    g.drawFittedText (getName(), boxArea, Justification::centred, 2);
}

void AudioProcessorGraphEditor::NodeView::resized()
{
    //auto b = getLocalBounds();
}

void AudioProcessorGraphEditor::NodeView::mouseDown (const MouseEvent& e)
{
    dragStart = e.getPosition();
}

void AudioProcessorGraphEditor::NodeView::mouseDrag (const MouseEvent& e)
{
    auto pos = getBounds().getTopLeft();
    auto delta = e.getPosition() - dragStart;
    setTopLeftPosition (pos + delta);

    if (node != nullptr)
    {
        // owner.nodeMoved (node->nodeID.uid, getBounds().getTopLeft());
    }
}

//==============================================================================
AudioProcessorGraphEditor::AudioProcessorGraphEditor (AudioProcessorGraph& g,
                                                      AudioPluginFormatManager& apfm,
                                                      KnownPluginList& kpl) :
    graph (g),
    formatManager (apfm),
    knownPlugins (kpl)
{
    graph.addChangeListener (this);
    viewport.setViewedComponent (&house, false);
    addAndMakeVisible (viewport);

    rebuild();
}

AudioProcessorGraphEditor::~AudioProcessorGraphEditor()
{
    graph.removeChangeListener (this);
    viewport.setViewedComponent (nullptr, false);
}

bool AudioProcessorGraphEditor::isOnTouchDevice()
{
    return Desktop::getInstance().getMainMouseSource().isTouch();
}

//==============================================================================
void AudioProcessorGraphEditor::setNodePosition (NodeID nodeID, juce::Point<double> pos)
{
    if (auto* n = graph.getNodeForId (nodeID))
    {
        n->properties.set ("x", jlimit (0.0, 1.0, pos.x));
        n->properties.set ("y", jlimit (0.0, 1.0, pos.y));
    }
}

juce::Point<double> AudioProcessorGraphEditor::getNodePosition (NodeID nodeID) const
{
    if (auto* n = graph.getNodeForId (nodeID))
        return { static_cast<double> (n->properties["x"]),
                 static_cast<double> (n->properties["y"]) };

    return {};
}

void AudioProcessorGraphEditor::clear()
{
    closeAnyOpenPluginWindows();
    graph.clear();
    changed();
}

bool AudioProcessorGraphEditor::closeAnyOpenPluginWindows()
{
    const bool wasEmpty = activePluginWindows.isEmpty();
    activePluginWindows.clear();
    return ! wasEmpty;
}

void AudioProcessorGraphEditor::rebuild()
{
    updateComponents();
}

void AudioProcessorGraphEditor::changed()
{ 
}

//==============================================================================
void AudioProcessorGraphEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void AudioProcessorGraphEditor::resized()
{
    viewport.setBounds (getLocalBounds());
    updateComponents();
}

void AudioProcessorGraphEditor::changeListenerCallback (ChangeBroadcaster*)
{
    updateComponents();
}

void AudioProcessorGraphEditor::timerCallback()
{
    // this should only be called on touch devices
    jassert (isOnTouchDevice());

    stopTimer();
    showPopupMenu (originalTouchPos);
}

void AudioProcessorGraphEditor::mouseDown (const MouseEvent& e)
{
    if (isOnTouchDevice())
    {
        originalTouchPos = e.position.toInt();
        startTimer (750);
    }

    if (e.mods.isPopupMenu())
        showPopupMenu (e.position.toInt());
}

void AudioProcessorGraphEditor::mouseUp (const MouseEvent&)
{
    if (isOnTouchDevice())
    {
        stopTimer();
        callAfterDelay (250, []() { PopupMenu::dismissAllActiveMenus(); });
    }
}

void AudioProcessorGraphEditor::mouseDrag (const MouseEvent& e)
{
    if (isOnTouchDevice() && e.getDistanceFromDragStart() > 5)
        stopTimer();
}

void AudioProcessorGraphEditor::createNewPlugin (juce::Point<int> position)
{
    // graph.addPlugin (desc, position.toDouble() / Point<double> ((double) getWidth(), (double) getHeight()));
}

AudioProcessorGraphEditor::NodeView* AudioProcessorGraphEditor::getComponentForPlugin (NodeID nodeID) const
{
    for (auto* nv : nodeViews)
        if (auto node = nv->node)
            if (node->nodeID == nodeID)
                return nv;

    return nullptr;
}

AudioProcessorGraphEditor::ConnectorComponent* AudioProcessorGraphEditor::getComponentForConnection (const Connection& conn) const
{
    for (auto* cc : connectors)
        if (cc->connection == conn)
            return cc;

    return nullptr;
}

AudioProcessorGraphEditor::PinComponent* AudioProcessorGraphEditor::findPinAt (juce::Point<float> pos) const
{
    for (auto* fc : nodeViews)
    {
        // NB: A Visual Studio optimiser error means we have to put this Component* in a local
        // variable before trying to cast it, or it gets mysteriously optimised away..
        auto* comp = fc->getComponentAt (pos.toInt() - fc->getPosition());

        if (auto* pin = dynamic_cast<PinComponent*> (comp))
            return pin;
    }

    return nullptr;
}

void AudioProcessorGraphEditor::beginConnectorDrag (NodeAndChannel source,  NodeAndChannel dest, const MouseEvent& e)
{
    auto* c = dynamic_cast<ConnectorComponent*> (e.originalComponent);
    connectors.removeObject (c, false);
    draggingConnector.reset (c);

    if (draggingConnector == nullptr)
        draggingConnector.reset (new ConnectorComponent (*this));

    draggingConnector->setInput (source);
    draggingConnector->setOutput (dest);

    house.addAndMakeVisible (draggingConnector.get());
    draggingConnector->toFront (false);

    dragConnector (e);
}

void AudioProcessorGraphEditor::dragConnector (const MouseEvent& e)
{
    auto e2 = e.getEventRelativeTo (this);

    if (draggingConnector != nullptr)
    {
        draggingConnector->setTooltip ({});

        auto pos = e2.position;

        if (auto* pin = findPinAt (pos))
        {
            auto connection = draggingConnector->connection;

            if (connection.source.nodeID == AudioProcessorGraph::NodeID() && ! pin->isInput)
            {
                connection.source = pin->pin;
            }
            else if (connection.destination.nodeID == AudioProcessorGraph::NodeID() && pin->isInput)
            {
                connection.destination = pin->pin;
            }

            if (graph.canConnect (connection))
            {
                pos = (pin->getParentComponent()->getPosition() + pin->getBounds().getCentre()).toFloat();
                draggingConnector->setTooltip (pin->getTooltip());
            }
        }

        if (draggingConnector->connection.source.nodeID == AudioProcessorGraph::NodeID())
            draggingConnector->dragStart (pos);
        else
            draggingConnector->dragEnd (pos);
    }
}

void AudioProcessorGraphEditor::endDraggingConnector (const MouseEvent& e)
{
    if (draggingConnector == nullptr)
        return;

    draggingConnector->setTooltip ({});

    auto e2 = e.getEventRelativeTo (this);
    auto connection = draggingConnector->connection;

    draggingConnector = nullptr;

    if (auto* pin = findPinAt (e2.position))
    {
        if (connection.source.nodeID == AudioProcessorGraph::NodeID())
        {
            if (pin->isInput)
                return;

            connection.source = pin->pin;
        }
        else
        {
            if (! pin->isInput)
                return;

            connection.destination = pin->pin;
        }

        graph.addConnection (connection);
    }
}

void AudioProcessorGraphEditor::updateComponents()
{
    for (int i = nodeViews.size(); --i >= 0;)
        if (graph.getNodeForId (nodeViews.getUnchecked (i)->node->nodeID) == nullptr)
            nodeViews.remove (i);

    for (int i = connectors.size(); --i >= 0;)
        if (! graph.isConnected (connectors.getUnchecked (i)->connection))
            connectors.remove (i);

    for (auto* fc : nodeViews)
        fc->update();

    for (auto* cc : connectors)
        cc->update();

    {
        const auto& nodes = graph.getNodes();
        for (int i = 0; i < nodes.size(); ++i)
        {
            auto node = nodes.getUnchecked (i);
            if (getComponentForPlugin (node->nodeID) == nullptr)
            {
                auto* comp = nodeViews.add (new NodeView (*this, node));
                house.addAndMakeVisible (comp);
                comp->update();
            }
        }
    }

    for (auto& c : graph.getConnections())
    {
        if (getComponentForConnection (c) == nullptr)
        {
            auto* comp = connectors.add (new ConnectorComponent (*this));
            house.addAndMakeVisible (comp);

            comp->setInput (c.source);
            comp->setOutput (c.destination);
        }
    }
}
