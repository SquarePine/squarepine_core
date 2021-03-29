/** Use this processor to apply dither to any incoming audio channels. */
class DitherProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    DitherProcessor() = default;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Basic Dither"); }
    /** @internal */
    Identifier getIdentifier() const override { return "BasicDither"; }
    /** @internal */
    void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock) override;
    /** @internal */
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    OwnedArray<BasicDither> dithers;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DitherProcessor)
};
