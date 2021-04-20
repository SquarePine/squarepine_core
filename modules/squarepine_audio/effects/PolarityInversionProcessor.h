//==============================================================================
/** Call this on an audio buffer to invert its polarity. */
template<typename FloatType>
inline void invertPolarity (AudioBuffer<FloatType>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    auto channels = buffer.getArrayOfWritePointers();

    for (auto i = buffer.getNumChannels(); --i >= 0;)
        FloatVectorOperations::multiply (channels[i], static_cast<FloatType> (-1), numSamples);
}

//==============================================================================
/** A simple processor that will flip the polarity to all incoming audio channels! */
class PolarityInversionProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    PolarityInversionProcessor();

    //==============================================================================
    /** Enables or disables the PolarityInversionProcessor */
    void setActive (bool shouldBeActive);

    /** Find out of an instance of a PolarityInversionProcessor is active */
    bool isActive() const;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Polarity Inverter"); }
    /** @internal */
    Identifier getIdentifier() const override { return "PolarityInverter"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    /** @internal */
    void processBlock (AudioBuffer<double>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    class InvertParameter;
    InvertParameter* invertParameter = nullptr;

    template<typename FloatType>
    void process (AudioBuffer<FloatType>& buffer, MidiBuffer&)
    {
        bool localActive = false;

        {
            const ScopedLock sl (getCallbackLock());
            localActive = isActive();
        }

        if (localActive)
            invertPolarity (buffer);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolarityInversionProcessor)
};
