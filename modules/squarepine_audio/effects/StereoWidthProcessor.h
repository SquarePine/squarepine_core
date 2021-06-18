/** Use this processor to apply stereo-width! */
class StereoWidthProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    StereoWidthProcessor();

    //==============================================================================
    /** Changes the stereo width.

        @param newWidth New stereo width amount.

        @see getMinimum, getNormal, getMaximum
    */
    void setWidth (float newWidth);

    /** @returns the current stereo width. */
    float getWidth() const;

    //==============================================================================
    /** */
    static constexpr float getMinimum() noexcept    { return 0.0f; }

    /** */
    static constexpr float getNormal() noexcept     { return 0.2f; }

    /** */
    static constexpr float getMaximum() noexcept    { return 1.0f; }

    //==============================================================================
    CREATE_INLINE_CLASS_IDENTIFIER (stereoWidth)

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return NEEDS_TRANS ("Stereo Width"); }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;

private:
    //==============================================================================
    AudioParameterFloat* widthParameter = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoWidthProcessor)
};
