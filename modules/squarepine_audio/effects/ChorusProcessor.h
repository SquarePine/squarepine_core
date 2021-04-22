/** */
class ChorusProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    ChorusProcessor();

    //==============================================================================
    /** Sets the rate (in Hz) of the LFO modulating the chorus delay line.
        This rate must be lower than 100 Hz.
    */
    void setRate (float newRateHz);

    /** @returns the current rate, in Hz. */
    float getRate() const noexcept;

    /** Sets the volume of the LFO modulating the chorus delay line (between 0 and 1). */
    void setDepth (float newDepth);

    /** @returns the current depth. */
    float getDepth() const noexcept;

    /** Sets the centre delay in milliseconds of the chorus delay line modulation.
        This delay must be between 1 and 100 ms.
    */
    void setCentreDelay (float newDelayMs);

    /** @returns the current centre delay. */
    float getCentreDelay() const noexcept;

    /** Sets the feedback volume (between -1 and 1) of the chorus delay line.
        Negative values can be used to get specific chorus sounds.
    */
    void setFeedback (float newFeedback);

    /** @returns the current feedback for the delay line. */
    float getFeedback() const noexcept;

    /** Sets the amount of dry and wet signal in the output of the chorus,
        between 0 for full dry and 1 for full wet.
    */
    void setMix (float newMix);

    /** @returns the current mix. */
    float getMix() const noexcept;

    //==============================================================================
    /** @internal */
    void prepareToPlay (double sampleRate, int bufferSize) override;
    /** @internal */
    void releaseResources() override;
    /** @internal */
    const String getName() const override { return "Chorus"; }
    /** @internal */
    Identifier getIdentifier() const override { return "Chorus"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    dsp::Chorus<float> floatChorus;
    dsp::Chorus<double> doubleChorus;

    using FloatParam = AudioParameterFloat*;
    FloatParam rate = nullptr, depth = nullptr,
               centreDelay = nullptr, feedback = nullptr,
               mix = nullptr;

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& buffer, dsp::Chorus<FloatType>& chorus)
    {
        const ScopedLock sl (getCallbackLock());

        dsp::AudioBlock<FloatType> block (buffer);
        dsp::ProcessContextReplacing<FloatType> context (block);
        chorus.process (context);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusProcessor)
};
