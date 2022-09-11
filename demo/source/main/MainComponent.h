#pragma once

/** */
class MainComponent final : public juce::Component
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

private:
    //==============================================================================
    HighPerformanceRendererConfigurator rendererConfigurator;
    TabbedComponent tabbedComponent;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
