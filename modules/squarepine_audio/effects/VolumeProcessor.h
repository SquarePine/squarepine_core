/** Use this processor to scale volumes of incoming audio samples! */
class VolumeProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    VolumeProcessor();

    //==============================================================================
    /** Change the volume

        A value of 1.0 means no volume change.
        To boost, pass in values beyond 1.0.
        To cut, pass in values under 1.0.

        @warning The value will be clamped to 0.0 if below 0.0.
        @warning Be careful when boosting - you may end-up with heavy distortion!

        @param newVolume New volume amount
    */
    void setVolume (float newVolume);

    /** Obtain the currently set volume

        @return Volume
    */
    float getVolume() const;

    /** */
    static constexpr auto maximumVolume = 5.0f;

    //==============================================================================
    CREATE_INLINE_CLASS_IDENTIFIER (volume)

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Volume"); }
    /** @internal */
    Identifier getIdentifier() const override { return volumeId; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    AudioParameterFloat* volumeParameter = nullptr;

    // We must track gain separately to the parameter so that we can ramp to the new parameter value
    float currentGain = 1.0f;

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& buffer, MidiBuffer&)
    {
        const ScopedLock sl (getCallbackLock());

        if (isBypassed())
            return;

        const auto localGain = volumeParameter->get();

        if (! approximatelyEqual (currentGain, localGain))
            buffer.applyGainRamp (0, buffer.getNumSamples(), (FloatType) currentGain, (FloatType) localGain);
        else
            buffer.applyGain ((FloatType) localGain);

        currentGain = localGain;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumeProcessor)
};
