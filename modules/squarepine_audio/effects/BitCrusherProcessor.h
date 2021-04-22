/** Use this processor to dynamically crush the audio's bits. */
class BitCrusherProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    BitCrusherProcessor();

    //==============================================================================
    /** */
    void setBitDepth (int newBitDepth);

    /** */
    int getBitDepth() const noexcept;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("BitCrusher"); }
    /** @internal */
    Identifier getIdentifier() const override { return "BitCrusher"; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    AudioParameterInt* bitDepth = new AudioParameterInt ("bitDepth", "Bit-Depth", 1, 32, 32);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitCrusherProcessor)
};
