/** Use this processor to apply panning to stereo channels! */
class PanProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    PanProcessor();

    //==============================================================================
    /** Changes the pan.

        This will be clamped to fullLeft and fullRight.

        @param newPan New pan amount.
    */
    void setPan (float newPan);

    /** @returns the current pan */
    [[nodiscard]] float getPan() const noexcept;

    /** Full left. */
    static constexpr auto fullLeft = -1.0f;

    /** Centre (no panning). */
    static constexpr auto centre = 0.0f;

    /** Full right. */
    static constexpr auto fullRight = 1.0f;

    //==============================================================================
    /** Change the panning rule. */
    void setPannerRule (dsp::PannerRule newRule);

    /** @returns the current panning rule. */
    [[nodiscard]] dsp::PannerRule getPannerRule() const noexcept;

    /** The default panning rule. */
    static constexpr auto defaultPannerRule = dsp::PannerRule::squareRoot3dB;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Stereophonic Panner"); }
    /** @internal */
    Identifier getIdentifier() const override { return "stereoPanner"; }
    /** @internal */
    bool acceptsMidi() const override { return true; }
    /** @internal */
    bool producesMidi() const override { return true; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

private:
    //==============================================================================
    class PanParameter;
    class PanRuleParameter;

    PanParameter* panParam = nullptr;
    PanRuleParameter* panRuleParam = nullptr;
    dsp::Panner<float> floatPanner;
    dsp::Panner<double> doublePanner;

    //==============================================================================
    template<typename FloatType>
    void process (dsp::Panner<FloatType>& panner,
                  juce::AudioBuffer<FloatType>& buffer,
                  MidiBuffer& midiMessages);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanProcessor)
};
