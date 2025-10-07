/** */
class SimplePhaserProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimplePhaserProcessor();

    //==============================================================================
    /** Sets the rate (in Hz) of the LFO modulating the phaser all-pass filters.
        This rate must be lower than 100 Hz.
    */
    void setRate (float);

    /** @returns the current rate, in Hz. */
    [[nodiscard]] float getRate() const;

    /** Sets the volume (between 0 and 1) of the
        LFO modulating the phaser all-pass filters.
    */
    void setDepth (float);

    /** @returns the current depth. */
    [[nodiscard]] float getDepth() const;

    /** Sets the centre frequency (in Hz) of the phaser's
        all-pass filters modulation.
    */
    void setCentreFrequency (float);

    /** @returns the current centre frequency. */
    [[nodiscard]] float getCentreFrequency() const;

    /** Sets the feedback volume (between -1 and 1) of the phaser.
        Negative can be used to get specific phaser sounds.
    */
    void setFeedback (float);

    /** @returns the current feedback for the delay line. */
    [[nodiscard]] float getFeedback() const;

    /** Sets the amount of dry and wet signal in the output of the phaser
        (between 0 for full dry and 1 for full wet).
    */
    void setMix (float);

    /** @returns the current mix. */
    [[nodiscard]] float getMix() const;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Simple Phaser"); }
    /** @internal */
    Identifier getIdentifier() const override { return "phaser"; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void releaseResources() override;
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

private:
    //==============================================================================
    dsp::Phaser<float> floatPhaser;
    dsp::Phaser<double> doublePhaser;

    using FloatParam = AudioParameterFloat*;
    FloatParam rate = nullptr, depth = nullptr,
               centreFrequency = nullptr, feedback = nullptr,
               mix = nullptr;

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, dsp::Phaser<FloatType>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimplePhaserProcessor)
};
