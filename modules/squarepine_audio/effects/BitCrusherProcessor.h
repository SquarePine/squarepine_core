/** @returns */
template<typename FloatType>
inline FloatType crushSample (FloatType sample, int targetBits)
{
    const FloatType dig = [&]()
    {
        if (std::is_same_v<FloatType, float>) return 32;
        else return 64;
    }();

    constexpr auto two  = static_cast<FloatType> (2);
    const auto x        = std::pow (two, static_cast<FloatType> (targetBits) / dig);
    const auto quant    = x / two;
    const auto dequant  = static_cast<FloatType> (1) / quant;
    return static_cast<FloatType> (dequant * (int)(sample * quant));
}

/** Use this processor to dynamically crush the audio's bits. */
class BitCrusherProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    BitCrusherProcessor();

    //==============================================================================
    /** */
    void setBitDepth (int newBitDepth);

    /** */
    [[nodiscard]] int getBitDepth() const noexcept;

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
