/** Use this processor to apply a basic EQ to all incoming audio channels. */
class SimpleEQProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleEQProcessor();

    /** Destructor. */
    ~SimpleEQProcessor() override;

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return NEEDS_TRANS ("Equaliser"); }
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
    using FilterType = dsp::StateVariableTPTFilterType;

    template<typename SampleType>
    using Context = dsp::ProcessContextReplacing<SampleType>;

    template<typename SampleType>
    using Filter = dsp::IIR::Filter<SampleType>;

    template<typename SampleType>
    using Coefficients = dsp::IIR::Coefficients<SampleType>;

    template<typename SampleType>
    using ProcessorDuplicator = dsp::ProcessorDuplicator<Filter<SampleType>, Coefficients<SampleType>>;

    class InternalFilter;
    OwnedArray<InternalFilter> filters;

    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    template<typename SampleType>
    void process (juce::AudioBuffer<SampleType>& buffer);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQProcessor)
};
