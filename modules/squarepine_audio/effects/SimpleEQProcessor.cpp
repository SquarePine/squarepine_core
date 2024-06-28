//==============================================================================
class SimpleEQProcessor::InternalFilter final : public AudioProcessorParameter::Listener
{
public:
    InternalFilter (FilterType filterType,
                    AudioParameterBool* b,
                    AudioParameterFloat* g,
                    AudioParameterFloat* c,
                    AudioParameterFloat* r) :
        type (filterType),
        bypass (b),
        gain (g),
        cutoff (c),
        resonance (r)
    {
        updateParams();

        bypass->addListener (this);
        gain->addListener (this);
        cutoff->addListener (this);
        resonance->addListener (this);
    }

    ~InternalFilter() override
    {
        bypass->removeListener (this);
        gain->removeListener (this);
        cutoff->removeListener (this);
        resonance->removeListener (this);
    }

    //==============================================================================
    void prepare (double newSampleRate, int bufferSize, int numChannels)
    {
        sampleRate = newSampleRate;

        const dsp::ProcessSpec spec
        {
            sampleRate,
            (uint32) std::max (0, bufferSize),
            (uint32) std::max (0, numChannels)
        };

        floatPackage.processor.prepare (spec);
        doublePackage.processor.prepare (spec);

        updateParams();
    }

    void reset()
    {
        floatPackage.processor.reset();
        doublePackage.processor.reset();
    }

    void updateParams()
    {
        updateParamsFor (floatPackage);
        updateParamsFor (doublePackage);
    }

    auto getMagnitudeDbFromFrequency (float frequency) const
    {
        jassert (sampleRate > 0.0);

        if (bypass->get())
            return 0.0f;

        return (float) doublePackage.displayState.getMagnitudeForFrequency ((double) frequency, sampleRate);
    }

    void parameterValueChanged (int, float) override    { updateParams(); }
    void parameterGestureChanged (int, bool) override   { }

    void process (juce::AudioBuffer<float>& buffer)     { process (floatPackage, buffer); }
    void process (juce::AudioBuffer<double>& buffer)    { process (doublePackage, buffer); }

    //==============================================================================
    static inline constexpr float nonZeroMinimum = 0.00001f;

    const FilterType type = FilterType::bandpass;
    double sampleRate = 44100.0;

    template<typename SampleType>
    struct Package final
    {
        ProcessorDuplicator<SampleType> processor;
        Coefficients<SampleType> displayState;
    };

    Package<float> floatPackage;
    Package<double> doublePackage;

    // NB: These are owned by the parent EQ processor.
    AudioParameterBool* bypass = nullptr;
    AudioParameterFloat* gain = nullptr;
    AudioParameterFloat* cutoff = nullptr;
    AudioParameterFloat* resonance = nullptr;

private:
    //==============================================================================
    template<typename SampleType>
    void updateParamsFor (Package<SampleType>& package)
    {
        auto snapToNonZero = [] (SampleType v) noexcept
        {
            constexpr auto t = static_cast<SampleType> (nonZeroMinimum);
            return std::max (t, v);
        };

        const auto c = snapToNonZero (static_cast<SampleType> (cutoff->get()));
        const auto r = snapToNonZero (static_cast<SampleType> (resonance->get()));
        const auto g = snapToNonZero (static_cast<SampleType> (gain->get()));

        using Coeffs = Coefficients<SampleType>;
        typename Coeffs::Ptr coeffs;

        switch (type)
        {
            case FilterType::lowpass:   coeffs = Coeffs::makeHighShelf (sampleRate, c, r, g); break;
            case FilterType::highpass:  coeffs = Coeffs::makeLowShelf (sampleRate, c, r, g); break;
            case FilterType::bandpass:  coeffs = Coeffs::makePeakFilter (sampleRate, c, r, g); break;

            default:
                jassertfalse;
            break;
        };

        package.displayState.coefficients = coeffs->coefficients;
        package.processor.state->coefficients = coeffs->coefficients;
    }

    template<typename SampleType>
    void process (Package<SampleType>& package,
                  juce::AudioBuffer<SampleType>& buffer)
    {
        dsp::AudioBlock<SampleType> block (buffer);
        Context<SampleType> context (block);
        context.isBypassed = bypass->get();

        package.processor.process (context);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InternalFilter)
};

//==============================================================================
class SimpleEQProcessor::FilterBypassParameter final : public AudioParameterBool
{
public:
    FilterBypassParameter (StringRef id, StringRef fbpName, bool value) :
        AudioParameterBool (id, fbpName, value)
    {
    }

    //==============================================================================
    String getText (float v, int maximumStringLength) const override
    {
        return (approximatelyEqual (v, 0.0f) ? TRANS ("Active") : TRANS ("Bypassed"))
                .substring (0, maximumStringLength);
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterBypassParameter)
};

//==============================================================================
class SimpleEQProcessor::Program final
{
public: 
    Program() noexcept = default;
    Program (const Program&) noexcept = default;
    Program (Program&&) noexcept = default;
    Program& operator= (const Program&) noexcept = default;
    Program& operator= (Program&&) noexcept = default;
    ~Program() noexcept = default;

    struct Band final
    {
        explicit Band (FilterType type_, 
                       float gain_,
                       float cutoff_,
                       float q_ = 1.0f,
                       bool bypass_ = false) noexcept :
            type (type_),
            bypass (bypass_),
            gain (gain_),
            cutoff (cutoff_),
            q (q_)
        {
        }

        explicit Band (FilterType type_, 
                       float gain_,
                       int midiNoteCutoff,
                       float q_ = 1.0f) noexcept :
            Band (type_, gain_, (float) MidiMessage::getMidiNoteInHertz (midiNoteCutoff), q_)
        {
        }

        Band() noexcept = default;
        Band (const Band&) noexcept = default;
        Band (Band&&) noexcept = default;
        Band& operator= (const Band&) noexcept = default;
        Band& operator= (Band&&) noexcept = default;
        ~Band() noexcept = default;

        void applyTo (InternalFilter& filter) const
        {
            jassert (type == filter.type);

            filter.bypass->operator= (bypass);
            filter.cutoff->operator= (cutoff);
            filter.resonance->operator= (q);
            filter.gain->operator= (gain);
        }

        FilterType type = FilterType::bandpass;
        bool bypass = false;
        float gain = 1.0f,
              cutoff = 5000.0f,
              q = 1.0f;
    };

    void applyTo (SimpleEQProcessor& parent) const
    {
        const Band* bands[] =
        {
            &highpass,
            &bandpass1,
            &bandpass2,
            &bandpass3,
            &lowpass
        };

        auto& filts = parent.filters;

        jassert ((int) std::size (bands) == filts.size());

        for (int i = 0; i < filts.size(); ++i)
        {
            const auto& band = bands[i];
            auto filter = filts.getUnchecked (i);
            jassert (band->type == filter->type);
            band->applyTo (*filter);
        }
    }

    String name;
    Band highpass { FilterType::highpass, 1.0f, 36, 1.0f },
         bandpass1 { FilterType::bandpass, 1.0f, 48, 1.0f },
         bandpass2 { FilterType::bandpass, 1.0f, 72, 1.0f },
         bandpass3 { FilterType::bandpass, 1.0f, 96, 1.0f },
         lowpass { FilterType::lowpass, 1.0f, 120, 1.0f };
};

//==============================================================================
SimpleEQProcessor::SimpleEQProcessor() :
    InternalProcessor (false)
{
    resetAPVTSWithLayout (createParameterLayout());

    programs.add (new Program()); // TODO

    programs.getFirst()->applyTo (*this);
}

SimpleEQProcessor::~SimpleEQProcessor()
{
    masterReference.clear();
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout SimpleEQProcessor::createParameterLayout()
{
    struct Config final
    {
        String name;
        FilterType type = FilterType::bandpass;
        float frequency = 0.0f;
    };

    const Config configs[] =
    {
        { NEEDS_TRANS ("LowShelf"), FilterType::highpass, (float) MidiMessage::getMidiNoteInHertz (24) },
        { NEEDS_TRANS ("BandPass1"), FilterType::bandpass, (float) MidiMessage::getMidiNoteInHertz (48) },
        { NEEDS_TRANS ("BandPass2"), FilterType::bandpass, (float) MidiMessage::getMidiNoteInHertz (72) },
        { NEEDS_TRANS ("BandPass3"), FilterType::bandpass, (float) MidiMessage::getMidiNoteInHertz (120) },
        { NEEDS_TRANS ("HighShelf"), FilterType::lowpass, 19000.0f }
    };

    filters.ensureStorageAllocated ((int) std::size (configs));

    auto layout = createDefaultParameterLayout();

    for (const auto& c : configs)
    {
        auto bypass = std::make_unique<FilterBypassParameter> (String ("bypassXYZ").replace ("XYZ", c.name),
                                                               String (NEEDS_TRANS ("Bypass (XYZ)")).replace ("XYZ", c.name),
                                                               false);

        auto gain = std::make_unique<AudioParameterFloat> (String ("gainXYZ").replace ("XYZ", c.name),
                                                           String (NEEDS_TRANS ("Gain (XYZ)")).replace ("XYZ", c.name),
                                                           InternalFilter::nonZeroMinimum, maxGain, 1.0f);

        auto cutoff = std::make_unique<AudioParameterFloat> (String ("cutoffXYZ").replace ("XYZ", c.name),
                                                             String (NEEDS_TRANS ("Cutoff (XYZ)")).replace ("XYZ", c.name),
                                                             20.0f, 20000.0f, c.frequency);

        auto resonance = std::make_unique<AudioParameterFloat> (String ("qXYZ").replace ("XYZ", c.name),
                                                                String (NEEDS_TRANS ("Q (XYZ)")).replace ("XYZ", c.name),
                                                                InternalFilter::nonZeroMinimum, 10.0f, 1.0f / MathConstants<float>::sqrt2);

        filters.add (new InternalFilter (c.type, bypass.get(), gain.get(), cutoff.get(), resonance.get()));

        layout.add (std::move (bypass));
        layout.add (std::move (gain));
        layout.add (std::move (cutoff));
        layout.add (std::move (resonance));
    }

    filters.minimiseStorageOverheads();

    return layout;
}

//==============================================================================
void SimpleEQProcessor::prepareToPlay (const double newSampleRate, const int bufferSize)
{
    setRateAndBufferSizeDetails (newSampleRate, bufferSize);

    const auto numChans = std::max (getTotalNumInputChannels(), getTotalNumOutputChannels());

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

    if (! isWholeProcBypassed)
        for (auto* f : filters)
            f->process (buffer);
}

AudioProcessor::CurveData SimpleEQProcessor::getResponseCurve (CurveData::Type type) const
{
    if (type != CurveData::Type::EQ)
    {
        jassertfalse; // Unsupported!
        return {};
    }

    CurveData curveData;

    curveData.xRange = { 1.0f, 21000.0f };
    curveData.yRange = { Decibels::gainToDecibels (0.0f), maxDecibels };

    WeakReference<SimpleEQProcessor> ptr ((SimpleEQProcessor*) this);
    curveData.curve = [ptr] (float frequency)
    {
        if (ptr == nullptr)
            return Decibels::gainToDecibels (0.0f);

        // TODO
        return ptr->filters.getFirst()->getMagnitudeDbFromFrequency (frequency);
    };

    return curveData;
}

int SimpleEQProcessor::getNumPrograms()     { return programs.size(); }
int SimpleEQProcessor::getCurrentProgram()  { return currentProgramIndex; }

void SimpleEQProcessor::setCurrentProgram (int newProgramIndex)
{
    if (currentProgramIndex != newProgramIndex
        && isPositiveAndBelow (newProgramIndex, getNumPrograms()))
    {
        currentProgramIndex = newProgramIndex;
        programs[currentProgramIndex]->applyTo (*this);
    }
}

const String SimpleEQProcessor::getProgramName (int index)
{
    if (auto* program = programs[index])
        return program->name;

    return {};
}
