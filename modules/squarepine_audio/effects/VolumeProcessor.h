/** Use this processor to scale volumes of incoming audio samples. */
class VolumeProcessor final : public InternalProcessor,
                              public AudioProcessorParameter::Listener
{
public:
    /** Constructor. */
    VolumeProcessor();

    //==============================================================================
    /** Changes the volume.

        A value of 1.0 means no volume change.
        To boost, pass in values beyond 1.0.
        To cut, pass in values under 1.0.

        @param newVolume New volume amount

        @see maximumVolume
    */
    void setVolume (float newVolume);

    /** @returns the current volume. */
    float getVolume() const;

    /** This is the maximum volume that can be set to a VolumeProcessor.

        And, though it should be obvious - the minimum is 0.0f.
    */
    static constexpr auto maximumVolume = MathConstants<float>::twoPi;

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return NEEDS_TRANS ("volume"); }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;
    /** @internal */
    void parameterValueChanged (int, float) override;
    /** @internal */
    void parameterGestureChanged (int, bool) override;

private:
    //==============================================================================
    AudioParameterFloat* volumeParameter = nullptr;

    LinearSmoothedValue<float> floatGain { 1.0f };
    LinearSmoothedValue<double> doubleGain { 1.0 };

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& buffer, 
                  LinearSmoothedValue<FloatType>& gain);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumeProcessor)
};
