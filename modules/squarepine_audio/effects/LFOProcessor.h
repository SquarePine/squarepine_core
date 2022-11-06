namespace oscillatorFunctions
{
    /** */
    template<typename FloatType>
    inline constexpr FloatType triangle (FloatType phase)
    {
        return static_cast<FloatType> (1)
               - (static_cast<FloatType> (4) * cabs (phase - static_cast<FloatType> (0.5)));
    }

    /** */
    template<typename FloatType>
    inline constexpr FloatType ramp (FloatType phase)
    {
        return (phase + cabs (phase)) * static_cast<FloatType> (0.5);
    }

    /** */
    template<typename FloatType>
    inline constexpr FloatType saw (FloatType phase)
    {
        return (phase * static_cast<FloatType> (2)) - static_cast<FloatType> (1);
    }

    /** */
    template<typename FloatType>
    inline constexpr FloatType square (FloatType phase)
    {
        constexpr auto one = static_cast<FloatType> (1);
        return phase > static_cast<FloatType> (0) ? one : -one;
    }
}

/** */
class LFOProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    LFOProcessor();

    //==============================================================================
    /** */
    enum class LFOType
    {
        sine,
        cosine,
        tangent,
        triangle,
        ramp,
        sawtooth,
        square
    };

    /** */
    void setLFOType (LFOType);

    /** */
    LFOType getLFOType() const;

    /** */
    void setFrequencyHz (double);

    /** */
    void setFrequencyFromMidiNote (int);

    /** */
    double getFrequency() const;

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return "LFO"; }
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
    bool isFirstRun = true;

    juce::AudioBuffer<float> floatMulter;
    juce::AudioBuffer<double> doubleMulter;

    dsp::Oscillator<float> floatOsc;
    dsp::Oscillator<double> doubleOsc;

    class TypeParameter;
    AudioParameterChoice* type = nullptr;
    AudioParameterFloat* frequency = nullptr;

    //==============================================================================
    void setLFOType (LFOType, bool force);

    template<typename FloatType>
    void setLFOType (dsp::Oscillator<FloatType>&, LFOType);

    template<typename FloatType>
    void process (dsp::Oscillator<FloatType>&,
                  juce::AudioBuffer<FloatType>& multer,
                  juce::AudioBuffer<FloatType>& buffer);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOProcessor)
};
