#pragma once

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
    virtual void updateWithNewTranslations()
    {
        setName (TRANS (untranslatedName));
    }

    //==============================================================================
    /** */
    void languageChanged (const IETFLanguageFile&) override
    {
        SQUAREPINE_CRASH_TRACER
        updateWithNewTranslations();
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
