/** */
class SimpleDistortionProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleDistortionProcessor();

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Simple Distortion"); }
    /** @internal */
    Identifier getIdentifier() const override { return "simpleDistortion"; }
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
    class AmountParameter;
    AmountParameter* amountParam = nullptr;

    template<typename FloatType>
    class BufferPackage
    {
    public:
        BufferPackage() = default;

        void prepare (const dsp::ProcessSpec& spec)
        {
            wet.setSize ((int) spec.numChannels, (int) spec.maximumBlockSize, false, true, true);
            mixer.prepare (spec);
        }

        juce::AudioBuffer<FloatType> wet;
        dsp::DryWetMixer<FloatType> mixer;
    };

    BufferPackage<float> floatPackage;
    BufferPackage<double> doublePackage;

    //==============================================================================
    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, BufferPackage<FloatType>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDistortionProcessor)
};
