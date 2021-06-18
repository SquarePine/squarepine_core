/** */
class LFOProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    LFOProcessor();

    //==============================================================================
    /** */
    void setLFOType (LFO* newLfo);

    /** */
    void setFrequency (double newFrequency);

    /** */
    void setFrequencyFromMidiNote (int midiNote);

    /** */
    double getFrequency() const noexcept { return configuration.frequency; }

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return "LFO"; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    bool isInstrument() const override { return true; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;

private:
    //==============================================================================
    std::unique_ptr<LFO> lfo;
    LFO::Configuration configuration;

    AudioParameterDouble* frequency = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOProcessor)
};
