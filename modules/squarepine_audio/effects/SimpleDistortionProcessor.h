/** */
class SimpleDistortionProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    SimpleDistortionProcessor();

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return NEEDS_TRANS ("Simple Distortion"); }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    class AmountParameter;
    AmountParameter* amountParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDistortionProcessor)
};
