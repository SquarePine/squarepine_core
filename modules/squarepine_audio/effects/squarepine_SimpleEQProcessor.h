/** Use this processor to apply a basic EQ to all incoming audio channels.

    By default this contains 5 bands, in this order:
    - High-pass
    - 3 centred band-passes
    - Low-pass
*/
class SimpleEQProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleEQProcessor();

    /** Destructor. */
    ~SimpleEQProcessor() override;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Simple Equaliser"); }
    /** @internal */
    Identifier getIdentifier() const override { return "equaliser"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;
    /** @internal */
    int getNumPrograms() override;
    /** @internal */
    int getCurrentProgram() override;
    /** @internal */
    void setCurrentProgram (int) override;
    /** @internal */
    const String getProgramName (int) override;
    /** @internal */
    CurveData getResponseCurve (CurveData::Type) const override;

private:
    //==============================================================================
    using FilterType = dsp::StateVariableTPTFilterType;

    template<typename SampleType>
    using Context = dsp::ProcessContextReplacing<SampleType>;

    template<typename SampleType>
    using Filter = dsp::IIR::Filter<SampleType>;

    template<typename SampleType>
    using Coefficients = dsp::IIR::Coefficients<SampleType>;

    template<typename SampleType>
    using ProcessorDuplicator = dsp::ProcessorDuplicator<Filter<SampleType>, Coefficients<SampleType>>;

    class FilterBypassParameter;

    class InternalFilter;
    OwnedArray<InternalFilter> filters;

    class Program;
    friend class Program;
    OwnedArray<Program> programs;
    int currentProgramIndex = 0;

    static inline const auto maxDecibels = 18.0f;
    static inline const auto maxGain = Decibels::decibelsToGain (maxDecibels);

    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    template<typename SampleType>
    void process (juce::AudioBuffer<SampleType>& buffer);

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (SimpleEQProcessor)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQProcessor)
};
