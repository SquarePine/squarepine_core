/** Use this processor to apply a basic EQ to all incoming audio channels.

    By default this contains 5 bands, in this order:
    - High-pass
    - 3 centred band-passes
    - Low-pass
*/
class SimpleEQProcessor final : public InternalProcessor,
                                public AudioProcessorParameter::Listener
{
public:
    /** Constructor. */
    SimpleEQProcessor();

    /** Destructor. */
    ~SimpleEQProcessor() override;

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Equaliser"); }
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

    constexpr static const float LOWFREQCUTOFF  = 70.f;
    constexpr static const float MIDFREQPEAK    = 1200.f;
    constexpr static const float HIGHFREQCUTOFF = 13000.f;
    
    void parameterValueChanged (int, float) override;
    void parameterGestureChanged (int, bool) override   { }
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

    template<typename SampleType>
    using ExponentialSmoothing = SmoothedValue<SampleType, ValueSmoothingTypes::Multiplicative>;
    
    class InternalFilter;
    OwnedArray<InternalFilter> filters;

    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    template<typename SampleType>
    void process (juce::AudioBuffer<SampleType>& buffer);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQProcessor)
};
