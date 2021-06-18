/** Use this processor to apply dither to any incoming audio channels. */
class DitherProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    DitherProcessor() = default;

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return NEEDS_TRANS ("Basic Dither"); }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;

private:
    //==============================================================================
    OwnedArray<BasicDither> dithers;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DitherProcessor)
};
