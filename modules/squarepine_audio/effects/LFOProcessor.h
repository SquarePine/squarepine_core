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
    const String getName() const override;
    /** @internal */
    Identifier getIdentifier() const override;
    /** @internal */
    void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock) override;
    /** @internal */
    bool isInstrument() const override;
    /** @internal */
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    std::unique_ptr<LFO> lfo;
    LFO::Configuration configuration;

    AudioParameterDouble* frequency = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOProcessor)
};
