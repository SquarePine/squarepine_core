/** */
class SimpleDistortionProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleDistortionProcessor();

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Simple Distortion"); }
    /** @internal */
    Identifier getIdentifier() const override { return "simpleDistortion"; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    class AmountParameter;
    AmountParameter* amountParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDistortionProcessor)
};
