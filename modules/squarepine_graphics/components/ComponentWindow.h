/** A quick and dirty window that you can create to popup a Component
    (which will be owned by the window).

    If you need anything more complicated, just create your own
    window derivative and go from there.
*/
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
    /** @internal */
    void closeButtonPressed() override
    {
        if (onClose != nullptr)
            onClose();
    }

    /** @internal */
    void moved() override
    {
        DocumentWindow::resized();
    }

    /** @internal */
    void lookAndFeelChanged() override
    {
        setBackgroundColour (getBackgroundColour());
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentWindow)
};
