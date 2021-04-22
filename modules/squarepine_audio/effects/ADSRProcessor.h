/** */
class ADSRProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    ADSRProcessor();

    //==============================================================================
    /** @internal */
    const String getName() const override { return "ADSR"; }
    /** @internal */
    Identifier getIdentifier() const override { return "ADSR"; }
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
    ADSR adsr;

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& buffer)
    {
        const auto& params = getParameters();

        ADSR::Parameters adsrParams =
        {
            params.getUnchecked (0)->getValue(),
            params.getUnchecked (1)->getValue(),
            params.getUnchecked (2)->getValue(),
            params.getUnchecked (3)->getValue()
        };

        const ScopedLock sl (getCallbackLock());
        adsr.setParameters (adsrParams);
        adsr.applyEnvelopeToBuffer (buffer, 0, buffer.getNumSamples());
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRProcessor)
};
