/** Shows the component under the mouse and component hierarchy.
    It's useful for debugging!
*/
class ComponentViewer final : public Component
{
public:
    /** Constructor. */
    ComponentViewer();

    //==============================================================================
    /** @internal */
    void lookAndFeelChanged() override;
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void resized() override;

private:
    //==============================================================================
    class Snapshot;
    class ContentComponent;

    std::unique_ptr<ContentComponent> contentComponent;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentViewer)
};
