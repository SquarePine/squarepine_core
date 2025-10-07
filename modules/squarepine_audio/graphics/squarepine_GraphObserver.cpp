//==============================================================================
GraphObserver::NodeView::NodeView (AudioProcessorGraph::Node::Ptr n) :
    node (n)
{
    jassert (node != nullptr);
}

GraphObserver::NodeView::~NodeView()
{
}

void GraphObserver::NodeView::paint (Graphics& g)
{
    auto b = getLocalBounds();
    g.setColour (Colours::black);
    g.fillEllipse (b.toFloat());

    if (node != nullptr)
    {
        if (auto* p = node->getProcessor())
        {
            const auto i = p->getTotalNumInputChannels();
            const auto o = p->getTotalNumOutputChannels();

            if (i > 0 && o > 0) g.setColour (Colours::green);
            else if (i > 0)     g.setColour (Colours::purple);
            else if (o > 0)     g.setColour (Colours::cyan);
            else                g.setColour (Colours::red);
        }
        else
        {
            jassertfalse;
            g.setColour (Colours::red);
        }
    }
    else
    {
        g.setColour (Colours::red);
    }

    g.fillEllipse (b.reduced (3, 3).toFloat());
}

void GraphObserver::NodeView::resized()
{
    //auto b = getLocalBounds();
}

//==============================================================================
GraphObserver::GraphObserver (AudioProcessorGraph& g) :
    graph (g)
{
    setOpaque (true);

    graph.addChangeListener (this);
    viewport.setViewedComponent (&house, false);
    addAndMakeVisible (viewport);

    rebuild();
}

GraphObserver::~GraphObserver()
{
    viewport.setViewedComponent (nullptr, false);
    graph.removeChangeListener (this);
}

void GraphObserver::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void GraphObserver::resized()
{
    viewport.setBounds (getLocalBounds());
}

void GraphObserver::changeListenerCallback (ChangeBroadcaster*)
{
    rebuild();
}

void GraphObserver::getLayout (std::vector<std::vector<uint32>>&) const
{
    auto connections = graph.getConnections();

    //DFS:
    int largestChain = 0;
    for (const auto& c : connections)
    {
        ignoreUnused (c);
        ignoreUnused (largestChain);
        //visitors.addIfNotAlreadyThere (c.source.nodeID.uid);
        //visitors.addIfNotAlreadyThere (c.destination.nodeID.uid);
    }
}

void GraphObserver::rebuild()
{
    rebuildNodes();
    rebuildConnections();
}

void GraphObserver::rebuildNodes()
{
    house.removeAllChildren();

    std::vector<std::vector<uint32>> rowsColumns;
    getLayout (rowsColumns);

    for (int i = 0; i < graph.getNumNodes(); ++i)
    {
        if (auto node = graph.getNode (i))
        {
            if (auto* nv = nodeViews.add (new NodeView (node)))
            {
                ignoreUnused (nv);
            }
        }
    }
}

void GraphObserver::rebuildConnections()
{
}
