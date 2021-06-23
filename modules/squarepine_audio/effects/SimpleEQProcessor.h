/** Use this processor to apply a basic 3 band EQ to all incoming audio channels. */
class SimpleEQProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    SimpleEQProcessor();

    //==============================================================================
    /** @returns this processor's APVTS. */
    AudioProcessorValueTreeState* getAPVTS() const noexcept { return apvts.get(); }

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
    /** */
    using FilterType = dsp::StateVariableTPTFilterType;
    /** */
    template<typename SampleType>
    using Context = dsp::ProcessContextReplacing<SampleType>;
    /** */
    template<typename SampleType>
    using Filter = dsp::IIR::Filter<SampleType>;
    /** */
    template<typename SampleType>
    using Coefficients = dsp::IIR::Coefficients<SampleType>;
    /** */
    template<typename SampleType>
    using ProcessorDuplicator = dsp::ProcessorDuplicator<Filter<SampleType>, Coefficients<SampleType>>;

    //==============================================================================
    /** */
    struct InternalFilter final : public AudioProcessorParameter::Listener
    {
        //==============================================================================
        InternalFilter (FilterType filterType,
                        AudioParameterFloat* g,
                        AudioParameterFloat* c,
                        AudioParameterFloat* r) :
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
        AudioParameterFloat* gain = nullptr;
        AudioParameterFloat* cutoff = nullptr;
        AudioParameterFloat* resonance = nullptr;

    private:
        //==============================================================================
        template<typename SampleType>
        void updateParamsFor (ProcessorDuplicator<SampleType>& processor)
        {
            const auto g = (SampleType) gain->get();
            const auto c = (SampleType) cutoff->get();
            const auto r = (SampleType) resonance->get();

            using Coeffs = Coefficients<SampleType>;
            Coeffs::Ptr coeffs;

            switch (type)
            {
                case FilterType::lowpass:
                    coeffs = Coeffs::makeHighShelf (sampleRate, c, r, g);
                break;

                case FilterType::highpass:
                    coeffs = Coeffs::makeLowShelf (sampleRate, c, r, g);
                break;

                case FilterType::bandpass:
                    coeffs = Coeffs::makePeakFilter (sampleRate, c, r, g);
                break;

                default:
                    jassertfalse;
                break;
            };

            processor.state = *coeffs;
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
        JUCE_DECLARE_NON_COPYABLE (InternalFilter)
    };

    //==============================================================================
    std::unique_ptr<AudioProcessorValueTreeState> apvts;
    OwnedArray<InternalFilter> filters;

    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        struct Config final
        {
            String name;
            FilterType type = FilterType::bandpass;
            int note = 0;
        };

        const Config configs[] =
        {
            { NEEDS_TRANS ("Highpass"), FilterType::highpass, 36 },
            { NEEDS_TRANS ("Bandpass"), FilterType::bandpass, 72 },
            { NEEDS_TRANS ("Lowpass"), FilterType::lowpass, 108 }
        };

        AudioProcessorValueTreeState::ParameterLayout layout;

        for (const auto& c : configs)
        {
            auto gain = std::make_unique<AudioParameterFloat> (String ("gainXYZ").replace ("XYZ", c.name),
                                                               TRANS ("Gain (XYZ)").replace ("XYZ", TRANS (c.name)),
                                                               0.0f, MathConstants<float>::pi, 0.25f);

            auto cutoff = std::make_unique<AudioParameterFloat> (String ("cutoffXYZ").replace ("XYZ", c.name),
                                                                 TRANS ("Cutoff (XYZ)").replace ("XYZ", TRANS (c.name)),
                                                                 20.0f, 20000.0f, (float) MidiMessage::getMidiNoteInHertz (c.note));

            auto resonance = std::make_unique<AudioParameterFloat> (String ("qXYZ").replace ("XYZ", c.name),
                                                                    TRANS ("Q (XYZ)").replace ("XYZ", TRANS (c.name)),
                                                                    0.00001f, 10.0f, 1.0f / MathConstants<float>::sqrt2);

            filters.add (new InternalFilter (c.type, gain.get(), cutoff.get(), resonance.get()));

            layout.add (std::move (gain));
            layout.add (std::move (cutoff));
            layout.add (std::move (resonance));
        }

        return layout;
    }

    template<typename SampleType>
    void process (juce::AudioBuffer<SampleType>& buffer)
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQProcessor)
};
