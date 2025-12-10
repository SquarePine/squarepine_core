/** A simplistic distortion effect that applies a sigmoid distortion. */
class SimpleDistortionProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleDistortionProcessor();

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Simple Distortion"); }
    /** @internal */
    Identifier getIdentifier() const override { return "simpleDistortion"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

private:
    //==============================================================================
    AudioParameterFloat* amountParameter = nullptr;

    //==============================================================================
    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDistortionProcessor)
};
