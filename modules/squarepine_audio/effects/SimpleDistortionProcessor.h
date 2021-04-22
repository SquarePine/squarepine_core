/** */
class SimpleDistortionProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    SimpleDistortionProcessor();

    //==============================================================================
    /** @internal */
    const String getName() const override;
    /** @internal */
    Identifier getIdentifier() const override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    class AmountParameter;
    AmountParameter* amountParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDistortionProcessor)
};
