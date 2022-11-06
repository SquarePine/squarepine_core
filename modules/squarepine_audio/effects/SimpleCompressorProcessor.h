/** */
class SimpleCompressorProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleCompressorProcessor();

    //==============================================================================
    /** Sets the threshold in dB.*/
    void setThreshold (float);

    /** @returns the threshold in dB. */
    float getThreshold() const;

    /** Sets the ratio (must be higher or equal to 1).*/
    void setRatio (float);

    /** @returns the ratio. */
    float getRatio() const;

    /** Sets the attack time in milliseconds.*/
    void setAttack (float);

    /** @returns the attack time in milliseconds. */
    float getAttack() const;

    /** Sets the release time in milliseconds. */
    void setRelease (float);

    /** @returns the release time in milliseconds. */
    float getRelease() const;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Simple Compressor"); }
    /** @internal */
    Identifier getIdentifier() const override { return "compressor"; }
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
    dsp::Compressor<float> floatCompressor;
    dsp::Compressor<double> doubleCompressor;

    using FloatParam = AudioParameterFloat*;
    FloatParam threshold = nullptr, ratio = nullptr,
               attack = nullptr, release = nullptr;

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, dsp::Compressor<FloatType>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleCompressorProcessor)
};
