/** */
class MainComponent final : public juce::Component,
                            public sp::LanguageHandler::Listener
{
public:
    /** */
    MainComponent (SharedObjects& sharedObjects);
    /** */
    ~MainComponent() override;

    //==============================================================================
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void languageChanged (const IETFLanguageFile&) override;

private:
    //==============================================================================
    HighPerformanceRendererConfigurator rendererConfigurator;
    TabbedComponent tabbedComponent;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
