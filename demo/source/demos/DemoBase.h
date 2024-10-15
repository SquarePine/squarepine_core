/** */
class DemoBase : public Component,
                 public LanguageHandler::Listener
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

protected:
    //==============================================================================
    SharedObjects& sharedObjects;

    //==============================================================================
    enum
    {
        marginPx    = 4,
        barSizePx   = marginPx * 12
    };

private:
    //==============================================================================
    const String untranslatedName;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoBase)
};
