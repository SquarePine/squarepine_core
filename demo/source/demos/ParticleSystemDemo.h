/** */
class ParticleSystemDemo final : public DemoBase
{
public:
    /** */
    ParticleSystemDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Particle System"))
    {
        addAndMakeVisible (particleSystem);
    }

    //==============================================================================
    void resized() override
    {
        auto b = getLocalBounds().reduced (marginPx);
        particleSystem.setBounds (b);
    }

private:
    //==============================================================================
    ParticleSystemComponent particleSystem;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParticleSystemDemo)
};
