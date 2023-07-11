/** */
class ADSRProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    ADSRProcessor();

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return "ADSR"; }
    /** @internal */
    bool acceptsMidi() const override { return true; }
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
    void process (juce::AudioBuffer<FloatType>& buffer, MidiBuffer& midiBuffer)
    {
        if (isBypassed())
            return;

        const auto& params = getParameters();

        const ADSR::Parameters adsrParams =
        {
            params.getUnchecked (0)->getValue(),
            params.getUnchecked (1)->getValue(),
            params.getUnchecked (2)->getValue(),
            params.getUnchecked (3)->getValue()
        };

        adsr.setParameters (adsrParams);

        bool hasNoteOn = false, hasNoteOff = false;

        for (auto e : midiBuffer)
        {
            const auto message = e.getMessage();
            if (message.isNoteOn())
                hasNoteOn = true;
            else if (message.isNoteOff())
                hasNoteOff = true;
        }

        if (hasNoteOn)
            adsr.noteOn();
        else if (hasNoteOff)
            adsr.noteOff();

        adsr.applyEnvelopeToBuffer (buffer, 0, buffer.getNumSamples());
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRProcessor)
};
