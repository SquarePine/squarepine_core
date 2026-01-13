/** A general demonstration of a highly flexible list view system. */
class ListViewDemo final : public DemoBase
{
public:
    ListViewDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("ListView"))
    {
        scrollView = makeListView();
        scrollView->setModel (&demoListModel);
        addAndMakeVisible (scrollView.get());

        updateWithNewTranslations();
    }

    ~ListViewDemo() override
    {
        scrollView->setModel (nullptr);
    }

    //==============================================================================
    void updateWithNewTranslations() override
    {
        SQUAREPINE_CRASH_TRACER
        repaint();
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced (margin);

        {
            auto top = b.removeFromTop (margin * 4);
        }

        b.removeFromTop (margin);
        scrollView->setBounds (b);
    }

private:
    //===============================4===============================================
    enum { margin = 8 };

    class DemoListModel final : public ListModel
    {
    public:
        DemoListModel() = default;

        int getNumItems() const override { return 1000; }

        Component* getItemComponent (int index) override
        {
            auto* label = new Label();
            label->setColour (Label::ColourIds::backgroundColourId, Colours::red);
            label->setText ("Item " + String(index), dontSendNotification);
            return label;
        }
    };

    DemoListModel demoListModel;
    std::unique_ptr<ScrollView> scrollView;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListViewDemo)
};
