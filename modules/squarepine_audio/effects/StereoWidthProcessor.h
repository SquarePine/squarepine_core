/** Use this processor to apply stereo-width! */
class StereoWidthProcessor final : public InternalProcessor,
                                   public AudioProcessorParameter::Listener
{
public:
    /** Constructor. */
    StereoWidthProcessor();

    //==============================================================================
    /** Changes the stereo width.

        @param newWidth New stereo width amount.

        @see getMinimum, getNormal, getMaximum
    */
    void setWidth (float newWidth);

    /** @returns the current stereo width. */
    float getWidth() const noexcept;

    //==============================================================================
    /** */
    static constexpr float minimumValue = 0.0f;

    /** */
    static constexpr float defaultValue = 0.2f;

    /** */
    static constexpr float maximumValue = 1.0f;

    //==============================================================================
    CREATE_INLINE_CLASS_IDENTIFIER (stereoWidth)

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Stereo Width"); }
    /** @internal */
    Identifier getIdentifier() const override { return "stereoWidth"; }
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
    AudioParameterFloat* widthParameter = nullptr;

    LinearSmoothedValue<float> floatWidth { 1.0f };
    LinearSmoothedValue<double> doubleWidth { 1.0 };

    //==============================================================================
    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& buffer, 
                  LinearSmoothedValue<FloatType>& value);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoWidthProcessor)
};
