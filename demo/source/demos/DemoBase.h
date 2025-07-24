/** */
class DemoBase : public Component,
                 public LanguageHandler::Listener,
                 public ApplicationCommandTarget
{
public:
    /** */
    DemoBase (SharedObjects& sharedObjs,
              const String& untransName) :
        sharedObjects (sharedObjs),
        untranslatedName (untransName)
    {
        SQUAREPINE_CRASH_TRACER

        sharedObjects.languageHandler->addListener (this);

        SafePointer ptr (this);
        MessageManager::callAsync ([ptr]()
        {
            if (ptr != nullptr)
                ptr->updateWithNewTranslations();
        });
    }

    /** */
    ~DemoBase() override
    {
        SQUAREPINE_CRASH_TRACER

        sharedObjects.languageHandler->removeListener (this);
    }

    //==============================================================================
    /** */
    const String& getUntranslatedName() const noexcept { return untranslatedName; }

    /** */
    virtual void updateWithNewTranslations() {}

    //==============================================================================
    /** */
    void languageChanged (const IETFLanguageFile&) override
    {
        SQUAREPINE_CRASH_TRACER
        setName (TRANS (untranslatedName));
        updateWithNewTranslations();
    }

    ApplicationCommandTarget* getNextCommandTarget() override
    {
        return ApplicationCommandManager::findTargetForComponent (getParentComponent());
    }

    void getAllCommands (Array<CommandID>& commands) override
    {
        commands.clear();
    }

    void getCommandInfo (CommandID, ApplicationCommandInfo&) override
    {
    }

    bool perform (const InvocationInfo&) override
    {
        return true;
    }

protected:
    //==============================================================================
    SharedObjects& sharedObjects;

private:
    //==============================================================================
    const String untranslatedName;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoBase)
};

//==============================================================================
/** */
class DemoBaseWithToolbars : public DemoBase
{
public:
    /** */
    DemoBaseWithToolbars (SharedObjects& sharedObjs,
                          const String& untransName) :
        DemoBase (sharedObjs, untransName)
    {
    }

    //==============================================================================
    virtual std::unique_ptr<Component> createHeaderComponent() const { return nullptr; }
    virtual std::unique_ptr<Component> createFooterComponent() const { return nullptr; }

    //==============================================================================
    virtual void resizeBody (Rectangle<int> dimensions) = 0;

    //==============================================================================
    void resized() final
    { 
        if (headerComponent == nullptr)
            headerComponent = createHeaderComponent();

        if (footerComponent == nullptr)
            footerComponent = createFooterComponent();

        auto b = getLocalBounds();

        if (headerComponent != nullptr)
            headerComponent->setBounds (b.removeFromTop (dims::toolbarSizePx));

        if (footerComponent != nullptr)
            footerComponent->setBounds (b.removeFromBottom (dims::toolbarSizePx));

        resizeBody (b);
    }

private:
    //==============================================================================
    std::unique_ptr<Component> headerComponent, footerComponent;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoBaseWithToolbars)
};
