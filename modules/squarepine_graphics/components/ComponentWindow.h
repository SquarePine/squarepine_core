/** */
class ComponentWindow final : public DocumentWindow
{
public:
    /** Constructor. */
    ComponentWindow (const String& windowName, Component* componentToOwn, Colour c = Colours::lightgrey) :
        DocumentWindow (windowName, c, DocumentWindow::allButtons)
    {
        setOpaque (true);
        setUsingNativeTitleBar (true);
        setVisible (true);
        setResizable (true, false);
        setContentOwned (componentToOwn, true);
        centreWithSize (componentToOwn->getWidth(), componentToOwn->getHeight());
    }

    //==============================================================================
    /** Callback for when the user signals to close the window. */
    std::function<void ()> onClose;

    //==============================================================================
    void closeButtonPressed() override
    {
        if (onClose != nullptr)
            onClose();
    }

    void moved() override
    {
        DocumentWindow::resized();
    }

    void lookAndFeelChanged() override
    {
        setBackgroundColour (getBackgroundColour());
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentWindow)
};
