/** A quick and dirty window that you can create to popup a Component
    (which will be owned by the window).

    If you need anything more complicated, just create your own
    window derivative and go from there.
*/
class ComponentWindow final : public DocumentWindow
{
public:
    /** Constructor. */
    ComponentWindow (const String& windowName,
                     Component* componentToOwn,
                     Colour c = Colours::lightgrey,
                     int requiredButtons = DocumentWindow::allButtons) :
        DocumentWindow (windowName, c, requiredButtons)
    {
        // This isn't a window into Narnia or something;
        // you need a component.
        jassert (componentToOwn != nullptr);

        setOpaque (true);
        setUsingNativeTitleBar (true);
        setResizable (true, false);
        setContentOwned (componentToOwn, true);
        centreWithSize (componentToOwn->getWidth(), componentToOwn->getHeight());
        setVisible (true);
    }

    //==============================================================================
    /** @internal */
    void closeButtonPressed() override      { if (onClose != nullptr) onClose(); }
    /** @internal */
    void moved() override                   { DocumentWindow::resized(); }
    /** @internal */
    void lookAndFeelChanged() override      { setBackgroundColour (getBackgroundColour()); }

    //==============================================================================
    /** Callback for when the user signals to close the window. */
    std::function<void ()> onClose;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentWindow)
};
