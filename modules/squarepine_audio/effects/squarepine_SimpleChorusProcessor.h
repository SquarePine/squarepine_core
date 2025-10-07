/** */
class SimpleChorusProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleChorusProcessor();

    //==============================================================================
    /** Sets the rate (in Hz) of the LFO modulating the chorus delay line.
        This rate must be lower than 100 Hz.
    */
    void setRate (float);

    /** @returns the current rate, in Hz. */
    [[nodiscard]] float getRate() const;

    /** Sets the volume of the LFO modulating the chorus delay line (between 0 and 1). */
    void setDepth (float);

    /** @returns the current depth. */
    [[nodiscard]] float getDepth() const;

    /** Sets the centre delay in milliseconds of the chorus delay line modulation.
        This delay must be between 1 and 100 ms.
    */
    void setCentreDelay (float);

    /** @returns the current centre delay. */
    [[nodiscard]] float getCentreDelay() const;

    /** Sets the feedback volume (between -1 and 1) of the chorus delay line.
        Negative values can be used to get specific chorus sounds.
    */
    void setFeedback (float);

    /** @returns the current feedback for the delay line. */
    [[nodiscard]] float getFeedback() const;

    /** Sets the amount of dry and wet signal in the output of the chorus,
        between 0 for full dry and 1 for full wet.
    */
    void setMix (float);

    /** @returns the current mix. */
    [[nodiscard]] float getMix() const;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Simple Chorus"); }
    /** @internal */
    Identifier getIdentifier() const override { return "chorus"; }
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
    dsp::Chorus<float> floatChorus;
    dsp::Chorus<double> doubleChorus;

    using FloatParam = AudioParameterFloat*;
    FloatParam rate = nullptr, depth = nullptr,
               centreDelay = nullptr, feedback = nullptr,
               mix = nullptr;

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, dsp::Chorus<FloatType>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleChorusProcessor)
};
