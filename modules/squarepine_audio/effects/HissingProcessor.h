/** Use this processor to add periodic hissing over the provided audio. */
class HissingProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    HissingProcessor() = default;

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Periodic Hisser"); }
    /** @internal */
    Identifier getIdentifier() const override { return "periodicHisser"; }
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
    int blockCounter = 0, blocksBetweenHisses = 0,
        maxBlocksBetweenHisses = 0, blocksPerHiss = 0;
    double level = 0.0;
    juce::Random random;

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HissingProcessor)
};

