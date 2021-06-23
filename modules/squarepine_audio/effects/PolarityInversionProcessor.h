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
    PolarityInversionProcessor();

    //==============================================================================
    /** Enables or disables the polarity inversion. */
    void setActive (bool shouldBeActive);

    /** @returns true if polarity is reversed. */
    bool isActive() const;

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return NEEDS_TRANS ("Polarity Inverter"); }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

private:
    //==============================================================================
    class InvertParameter;
    InvertParameter* invertParameter = nullptr;

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& buffer, MidiBuffer&)
    {
        if (! isBypassed() && isActive())
            invertPolarity (buffer);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolarityInversionProcessor)
};
