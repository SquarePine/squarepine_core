/** Use this processor to apply stereo-width! */
class StereoWidthProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    StereoWidthProcessor();

    //==============================================================================
    /** Change the width

        @param newWidth New width amount.

        @see getMinimum, getNormal, getMaximum
    */
    void setWidth (float newWidth);

    /** Obtain the current stereo width */
    float getWidth() const;

    //==============================================================================
    /** */
    static constexpr float getMinimum() noexcept    { return 0.0f; }

    /** */
    static constexpr float getNormal() noexcept     { return 0.2f; }

    /** */
    static constexpr float getMaximum() noexcept    { return 1.0f; }

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Stereo Width"); }
    /** @internal */
    Identifier getIdentifier() const override { return "StereoWidth"; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    class WidthParameter;
    WidthParameter* widthParameter = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoWidthProcessor)
};
