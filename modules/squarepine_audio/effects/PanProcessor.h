/** Use this processor to apply panning to stereo channels! */
class PanProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    PanProcessor();

    //==============================================================================
    /** Change the pan.

        @param newPan New pan amount.
    */
    void setPan (float newPan);

    /** @returns the current pan */
    float getPan() const noexcept;

    //==============================================================================
    /** Change the panning rule. */
    void setPanLaw (dsp::PannerRule newRule);

    /** @returns the current panning rule. */
    dsp::PannerRule getPannerRule() const noexcept;

    //==============================================================================
    /** Full left. */
    static constexpr auto fullLeft = -1.0f;

    /** Centre (no panning). */
    static constexpr auto centre = 0.0f;

    /** Full right. */
    static constexpr auto fullRight = 1.0f;

    /** The default panning rule. */
    static constexpr auto defaultPannerRule = dsp::PannerRule::squareRoot3dB;

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return NEEDS_TRANS ("Stereophonic Panner"); }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    bool acceptsMidi() const override { return true; }
    /** @internal */
    bool producesMidi() const override { return true; }

private:
    //==============================================================================
    class PanParameter;
    class PanRuleParameter;

    PanParameter* panParam = nullptr;
    PanRuleParameter* panRuleParam = nullptr;

    //==============================================================================
    dsp::Panner<float> panner;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanProcessor)
};
