class JUCEReverbProcessor : public InternalProcessor
{
public:
    /** Constructor */
    JUCEReverbProcessor();

    //==============================================================================
    /** @internal */
    void prepareToPlay (double sampleRate, int bufferSize) override;
    /** @internal */
    void releaseResources() override;
    /** @internal */
    const String getName() const override;
    /** @internal */
    Identifier getIdentifier() const override;
    /** @internal */
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    void updateReverbParameters();

    //==============================================================================
    Reverb reverb;

    AudioParameterFloat* roomSize = nullptr;
    AudioParameterFloat* damping = nullptr;
    AudioParameterFloat* wetLevel = nullptr;
    AudioParameterFloat* dryLevel = nullptr;
    AudioParameterFloat* width = nullptr;
    AudioParameterBool* freezeMode = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JUCEReverbProcessor)
};
