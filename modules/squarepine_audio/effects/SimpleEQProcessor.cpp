//==============================================================================
class SimpleEQProcessor::InternalFilter final : public AudioProcessorParameter::Listener
{
public:
    InternalFilter (FilterType filterType,
                    NotifiableAudioParameterFloat* g,
                    NotifiableAudioParameterFloat* c,
                    NotifiableAudioParameterFloat* r) :
        type (filterType),
        gain (g),
        cutoff (c),
        resonance (r)
    {
        updateParams();

        gain->addListener (this);
        cutoff->addListener (this);
        resonance->addListener (this);
    }

    //==============================================================================
    void prepare (double newSampleRate, int bufferSize, int numChannels)
    {
        sampleRate = newSampleRate;

        const dsp::ProcessSpec spec
        {
            sampleRate,
            (uint32) jmax (0, bufferSize),
            (uint32) jmax (0, numChannels)
        };

        floatProcessor.prepare (spec);
        doubleProcessor.prepare (spec);

        updateParams();
    }

    void reset()
    {
        floatProcessor.reset();
        doubleProcessor.reset();
    }

    void process (juce::AudioBuffer<float>& buffer, bool wholeProcIsBypassed)
    {
        process (floatProcessor, buffer, wholeProcIsBypassed);
    }

    void process (juce::AudioBuffer<double>& buffer, bool wholeProcIsBypassed)
    {
        process (doubleProcessor, buffer, wholeProcIsBypassed);
    }

    void updateParams()
    {
        updateParamsFor (floatProcessor);
        updateParamsFor (doubleProcessor);
    }

    void parameterValueChanged (int, float) override    { updateParams(); }
    void parameterGestureChanged (int, bool) override   { }

    //==============================================================================
    const FilterType type = FilterType::bandpass;
    double sampleRate = 44100.0;
    ProcessorDuplicator<float> floatProcessor;
    ProcessorDuplicator<double> doubleProcessor;

    // NB: These are owned by the parent EQ processor.
    NotifiableAudioParameterFloat* gain = nullptr;
    NotifiableAudioParameterFloat* cutoff = nullptr;
    NotifiableAudioParameterFloat* resonance = nullptr;

private:
    //==============================================================================
    template<typename SampleType>
    void updateParamsFor (ProcessorDuplicator<SampleType>& processor)
    {
        const auto g = (SampleType) gain->get();
        const auto c = (SampleType) cutoff->get();
        const auto r = (SampleType) resonance->get();

        using Coeffs = Coefficients<SampleType>;
        typename Coeffs::Ptr coeffs;

        switch (type)
        {
            case FilterType::lowpass:   coeffs = Coeffs::makeLowShelf (sampleRate, c, r, g); break;
            case FilterType::highpass:  coeffs = Coeffs::makeHighShelf (sampleRate, c, r, g); break;
            case FilterType::bandpass:  coeffs = Coeffs::makePeakFilter (sampleRate, c, r, g); break;

            default:
                jassertfalse;
            break;
        };

        *processor.state = *coeffs;
    }

    template<typename SampleType>
    void process (ProcessorDuplicator<SampleType>& processor,
                  juce::AudioBuffer<SampleType>& buffer,
                  bool wholeProcIsBypassed)
    {
        dsp::AudioBlock<SampleType> block (buffer);
        Context<SampleType> context (block);
        context.isBypassed = wholeProcIsBypassed;

        processor.process (context);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InternalFilter)
};

//==============================================================================
SimpleEQProcessor::SimpleEQProcessor() :
    InternalProcessor (false)
{
    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", createParameterLayout()));
}

SimpleEQProcessor::~SimpleEQProcessor()
{
}

//==============================================================================
void SimpleEQProcessor::prepareToPlay (const double newSampleRate, const int bufferSize)
{
    setRateAndBufferSizeDetails (newSampleRate, bufferSize);

    const auto numChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels());

    const ScopedLock sl (getCallbackLock());

    for (auto* f : filters)
        f->prepare (newSampleRate, bufferSize, numChans);
}

void SimpleEQProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)    { process (buffer); }
void SimpleEQProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)   { process (buffer); }

template<typename SampleType>
void SimpleEQProcessor::process (juce::AudioBuffer<SampleType>& buffer)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    const bool isWholeProcBypassed = isBypassed()
                                  || buffer.hasBeenCleared()
                                  || numChannels <= 0
                                  || numSamples <= 0;

    const ScopedLock sl (getCallbackLock());

    for (auto* f : filters)
        f->process (buffer, isWholeProcBypassed);
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout SimpleEQProcessor::createParameterLayout()
{
    struct Config final
    {
        String name;
        FilterType type = FilterType::bandpass;
        int note = 0;
    };

    const Config configs[] =
    {
        { NEEDS_TRANS ("LowShelf"), FilterType::lowpass, 24 },
        { NEEDS_TRANS ("BandPass"), FilterType::bandpass, 60 },
        { NEEDS_TRANS ("HighShelf"), FilterType::highpass, 108 }
    };

    filters.ensureStorageAllocated (numElementsInArray (configs));

    auto layout = createDefaultParameterLayout();

    for (const auto& c : configs)
    {
        auto gain = std::make_unique<NotifiableAudioParameterFloat> (String ("gainXYZ").replace ("XYZ", c.name),
                                                                     TRANS ("Gain (XYZ)").replace ("XYZ", TRANS (c.name)),
                                                                     0.0f,
                                                                     MathConstants<float>::pi,
                                                                     1.0f,
                                                                     true);

        auto cutoff = std::make_unique<NotifiableAudioParameterFloat> (String ("cutoffXYZ").replace ("XYZ", c.name),
                                                                       TRANS ("Cutoff (XYZ)").replace ("XYZ", TRANS (c.name)),
                                                                       20.0f,
                                                                       20000.0f,
                                                                       (float) MidiMessage::getMidiNoteInHertz (c.note),
                                                                       false);

        auto resonance = std::make_unique<NotifiableAudioParameterFloat> (String ("qXYZ").replace ("XYZ", c.name),
                                                                          TRANS ("Q (XYZ)").replace ("XYZ", TRANS (c.name)),
                                                                          0.00001f,
                                                                          10.0f,
                                                                          1.0f / MathConstants<float>::sqrt2,
                                                                          false);

        filters.add (new InternalFilter (c.type, gain.get(), cutoff.get(), resonance.get()));

        layout.add (std::move (gain));
        layout.add (std::move (cutoff));
        layout.add (std::move (resonance));
    }

    filters.minimiseStorageOverheads();

    return layout;
}
