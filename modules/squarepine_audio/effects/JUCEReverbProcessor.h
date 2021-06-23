/** */
class JUCEReverbProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    JUCEReverbProcessor();

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return NEEDS_TRANS ("Simple Reverb"); }
    /** @internal */
    void prepareToPlay (double sampleRate, int bufferSize) override;
    /** @internal */
    void releaseResources() override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;

private:
    //==============================================================================
    Reverb reverb;

    AudioParameterFloat* roomSize = nullptr;
    AudioParameterFloat* damping = nullptr;
    AudioParameterFloat* wetLevel = nullptr;
    AudioParameterFloat* dryLevel = nullptr;
    AudioParameterFloat* width = nullptr;
    AudioParameterBool* freezeMode = nullptr;

    //==============================================================================
    void updateReverbParameters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JUCEReverbProcessor)
};
