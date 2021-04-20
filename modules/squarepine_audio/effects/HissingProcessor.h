/** Use this processor to add periodic hissing over the provided audio. */
class HissingProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    HissingProcessor() = default;

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Periodic Hisser"); }
    /** @internal */
    Identifier getIdentifier() const override { return "hisser"; }
    /** @internal */
    void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock) override;
    /** @internal */
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    int blockCounter = 0, blocksBetweenHisses = 0, maxBlocksBetweenHisses = 0, blocksPerHiss = 0;
    double level = 0;
    juce::Random random;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HissingProcessor)
};

