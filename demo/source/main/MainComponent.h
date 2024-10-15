/** */
class MainComponent final : public juce::Component,
                            public sp::LanguageHandler::Listener,
                            public MenuBarModel
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
    /** @internal */
    StringArray getMenuBarNames() override;
    /** @internal */
    PopupMenu getMenuForIndex (int, const String&) override;
    /** @internal */
    void menuItemSelected (int, int) override;
    /** @internal */
    void menuBarActivated (bool) override;

private:
    //==============================================================================
    HighPerformanceRendererConfigurator rendererConfigurator;
    OwnedArray<DemoBase> demos;
    DemoBase* activeDemo = nullptr;
    TabbedComponent tabbedComponent;
    BurgerMenuComponent burgerMenu;

    AnimatorSetBuilder animatorSetBuilder { 0.0 };
    VBlankAnimatorUpdater animatorUpdater { this };
    OffloadedTimer animationUpdater;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
