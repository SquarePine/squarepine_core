namespace djdawprocessor
{

/** Use this processor to dynamically crush the audio's bits. */
class BitCrusherProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    BitCrusherProcessor();
    
    //==============================================================================
    /** */
    void setBitDepth (float newBitDepth);
    
    /** */
    float getBitDepth() const noexcept;
    
    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("BitCrusher"); }
    /** @internal */
    Identifier getIdentifier() const override { return "bitCrusher"; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    
private:
    //==============================================================================
    AudioParameterFloat* bitDepth = new AudioParameterFloat ("bitDepth", "Bit-Depth", 1.f, 32.f, 32.f);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitCrusherProcessor)
};

}
