#pragma once

class MainComponent final : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    HighPerformanceRendererConfigurator rendererConfigurator;
    TabbedComponent tabbedComponent;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
