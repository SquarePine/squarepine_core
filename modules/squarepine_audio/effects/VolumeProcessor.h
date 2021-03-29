/** Use this processor to scale volumes of incoming audio samples! */
class VolumeProcessor : public InternalProcessor
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
    const String getName() const override;
    /** @internal */
    Identifier getIdentifier() const override { return volumeId; }
    /** @internal */
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    /** @internal */
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    AudioParameterFloat* volumeParameter = nullptr;

    // We must track gain separately to the parameter so that we can ramp to the new parameter value
    float currentGain = 1.0f;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumeProcessor)
};
