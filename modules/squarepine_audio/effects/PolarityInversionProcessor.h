//==============================================================================
/** Call this on an audio buffer to invert its polarity. */
template<typename FloatType>
inline void invertPolarity (juce::AudioBuffer<FloatType>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    auto channels = buffer.getArrayOfWritePointers();

    for (auto i = buffer.getNumChannels(); --i >= 0;)
        FloatVectorOperations::multiply (channels[i], static_cast<FloatType> (-1), numSamples);
}

//==============================================================================
/** A simple processor that will flip the polarity of all incoming audio channels! */
class PolarityInversionProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    PolarityInversionProcessor (bool startActive = false);

    //==============================================================================
    /** Enables or disables the polarity inversion. */
    void setActive (bool shouldBeActive);

    /** @returns true if the polarity is reversed. */
    bool isActive() const noexcept;

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Polarity Inverter"); }
    /** @internal */
    Identifier getIdentifier() const override { return "polarityInverter"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

private:
    //==============================================================================
    AudioParameterBool* invertParameter = nullptr;

    //==============================================================================
    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, MidiBuffer&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolarityInversionProcessor)
};
