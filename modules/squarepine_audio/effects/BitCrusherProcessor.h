/** Use this processor to dynamically crush the audio's bits. */
class BitCrusherProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    BitCrusherProcessor();

    //==============================================================================
    /** */
    void setBitDepth (int);

    /** */
    int getBitDepth() const noexcept;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Bit Crusher"); }
    /** @internal */
    Identifier getIdentifier() const override { return "bitCrusher"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

private:
    //==============================================================================
    AudioParameterInt* bitDepth = new AudioParameterInt ("bitDepth", "Bit-Depth", 1, 16, 8);

    //==============================================================================
    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitCrusherProcessor)
};
