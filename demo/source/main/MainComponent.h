/** */
class MainComponent final : public juce::Component,
                            public sp::LanguageHandler::Listener,
                            public MenuBarModel
{
public:
    /** */
    MainComponent (SharedObjects&);
    /** */
    ~MainComponent() override;

    //==============================================================================
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void languageChanged (const IETFLanguageFile&) override { updateTranslations(); }
    /** @internal */
    StringArray getMenuBarNames() override;
    /** @internal */
    PopupMenu getMenuForIndex (int, const String&) override;
    /** @internal */
    void menuItemSelected (int, int) override;
    /** @internal */
    void menuBarActivated (bool) override {}

private:
    //==============================================================================
    HighPerformanceRendererConfigurator rendererConfigurator;
    OwnedArray<DemoBase> demos;
    DemoBase* activeDemo = nullptr;

    juce::Rectangle<int> barArea;
    TextButton popupButton;

    AnimatorSetBuilder animatorSetBuilder { 0.0 };
    VBlankAnimatorUpdater animatorUpdater { this };
    OffloadedTimer animationUpdater;

    //==============================================================================
    void updateTranslations();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
