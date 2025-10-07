/** */
class SimpleReverbProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleReverbProcessor();

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Simple Reverb"); }
    /** @internal */
    Identifier getIdentifier() const override { return "simpleReverb"; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void releaseResources() override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;

private:
    //==============================================================================
    Reverb reverb;

    using FloatParam = AudioParameterFloat*;
    FloatParam roomSize = nullptr, damping = nullptr,
               wetLevel = nullptr, dryLevel = nullptr,
               width = nullptr;

    AudioParameterBool* freezeMode = nullptr;

    //==============================================================================
    void updateReverbParameters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleReverbProcessor)
};
