/** */
class SimpleLimiterProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleLimiterProcessor();

    //==============================================================================
    /** Sets the threshold in dB.*/
    void setThreshold (float);

    /** @returns the threshold in dB. */
    [[nodiscard]] float getThreshold() const;

    /** Sets the release time in milliseconds.*/
    void setRelease (float);

    /** @returns the release time in milliseconds. */
    [[nodiscard]] float getRelease() const;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Simple Limiter"); }
    /** @internal */
    Identifier getIdentifier() const override { return "limiter"; }
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
    dsp::Limiter<float> floatLimiter;
    dsp::Limiter<double> doubleLimiter;

    using FloatParam = AudioParameterFloat*;
    FloatParam threshold = nullptr, release = nullptr;

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, dsp::Limiter<FloatType>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleLimiterProcessor)
};
