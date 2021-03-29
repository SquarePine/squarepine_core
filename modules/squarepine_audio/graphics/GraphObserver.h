/** */
class GraphObserver final : public Component,
                            public ChangeListener
{
public:
    /** */
    GraphObserver (AudioProcessorGraph& g);
    /** */
    ~GraphObserver() override;

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void changeListenerCallback (ChangeBroadcaster* source) override;

private:
    //==============================================================================
    /** */
    class NodeView final : public Component
    {
    public:
        /** */
        NodeView (AudioProcessorGraph::Node::Ptr n);
        /** */
        ~NodeView() override;

        /** @internal */
        void paint (Graphics& g) override;
        /** @internal */
        void resized() override;

        AudioProcessorGraph::Node::Ptr node;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeView)
    };

    AudioProcessorGraph& graph;
    Component house;
    Viewport viewport;
    OwnedArray<NodeView> nodeViews;

    void getLayout (std::vector<std::vector<uint32>>& rowsColumns) const;
    void rebuild();
    void rebuildNodes();
    void rebuildConnections();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphObserver)
};
