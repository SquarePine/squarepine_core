//==============================================================================
class LFOProcessor::TypeParameter final : public AudioParameterChoice
{
public:
    TypeParameter() :
        AudioParameterChoice (ParameterID ("type", 1), TRANS ("Type"), getChoices(),
                              static_cast<int> (LFOProcessor::LFOType::sine))
    {
    }

private:
    static StringArray getChoices()
    {
        StringArray choices;
        choices.add (NEEDS_TRANS ("Sine"));
        choices.add (NEEDS_TRANS ("Cosine"));
        choices.add (NEEDS_TRANS ("Tangent"));
        choices.add (NEEDS_TRANS ("Triangle"));
        choices.add (NEEDS_TRANS ("Ramp"));
        choices.add (NEEDS_TRANS ("Sawtooth"));
        choices.add (NEEDS_TRANS ("Square"));
        return choices;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TypeParameter)
};

//==============================================================================
LFOProcessor::LFOProcessor (double minFreqHz, double maxFreqHz,
                            double defaultFreqHz, bool isMult) :
    InternalProcessor (false),
    isMultiplying (isMult)
{
    jassert (minFreqHz > 0.0f);
    jassert (maxFreqHz > minFreqHz);

    auto layout = createDefaultParameterLayout();

    auto tp = std::make_unique<TypeParameter>();
    type = tp.get();
    layout.add (std::move (tp));

    auto pf = std::make_unique<AudioParameterFloat> (ParameterID ("frequency", 1), "Frequency", minFreqHz, maxFreqHz, defaultFreqHz);
    frequency = pf.get();
    layout.add (std::move (pf));

    resetAPVTSWithLayout (std::move (layout));

    setLFOType (LFOType::sine);
}

//==============================================================================
template<typename FloatType>
void LFOProcessor::setLFOType (dsp::Oscillator<FloatType>& osc, LFOType lfoType)
{
    std::function<FloatType (FloatType)> func;

    switch (lfoType)
    {
        case LFOType::sine:             func = (FloatType (*) (FloatType)) &std::sin; break;
        case LFOType::cosine:           func = (FloatType (*) (FloatType)) &std::cos; break;
        case LFOType::tangent:          func = (FloatType (*) (FloatType)) &std::tan; break;
        case LFOType::triangle:         func = oscillatorFunctions::triangle<FloatType>; break;
        case LFOType::ramp:             func = oscillatorFunctions::ramp<FloatType>; break;
        case LFOType::sawtooth:         func = oscillatorFunctions::saw<FloatType>; break;
        case LFOType::square:           func = oscillatorFunctions::square<FloatType>; break;
        case LFOType::whiteNoise:       func = [this] (FloatType v) { return whiteNoiseGenerator.process (v); }; break;
        case LFOType::pinkNoise:        func = [this] (FloatType v) { return pinkNoiseGenerator.process (v); }; break;
        case LFOType::blueNoise:        func = [this] (FloatType v) { return blueNoiseGenerator.process (v); }; break;
        case LFOType::brownianNoise:    func = [this] (FloatType v) { return brownianNoiseGenerator.process (v); }; break;

        default:
            jassertfalse;
        break;
    };

    osc.initialise (func);
}

void LFOProcessor::setLFOType (LFOType lfoType, bool force)
{
    if (! force)
        if (type == nullptr || type->getIndex() == static_cast<int> (lfoType))
            return;

    setLFOType (floatOsc, lfoType);
    setLFOType (doubleOsc, lfoType);
}

void LFOProcessor::setLFOType (LFOType lfoType)             { setLFOType (lfoType, true); }
void LFOProcessor::setFrequencyHz (double newFrequency)     { *frequency = (float) newFrequency; }
void LFOProcessor::setFrequency (const Pitch& pitch)        { setFrequencyHz (pitch.getFrequencyHz()); }
void LFOProcessor::setFrequencyFromMidiNote (int midiNote)  { setFrequencyHz (MidiMessage::getMidiNoteInHertz (midiNote)); }
double LFOProcessor::getFrequencyHz() const                 { return frequency->get(); }
Pitch LFOProcessor::getFrequencyPitch() const               { return getFrequencyHz(); }
LFOProcessor::LFOType LFOProcessor::getLFOType() const      { return (LFOType) type->getIndex(); }

//==============================================================================
void LFOProcessor::prepareToPlay (const double newSampleRate, const int samplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, samplesPerBlock);

    const auto numChans = std::max (getTotalNumInputChannels(), getTotalNumOutputChannels());

    floatMulter.setSize (numChans, samplesPerBlock, false, true, true);
    doubleMulter.setSize (numChans, samplesPerBlock, false, true, true);

    const dsp::ProcessSpec spec =
    {
        newSampleRate,
        (uint32) samplesPerBlock,
        (uint32) numChans
    };

    floatOsc.prepare (spec);
    doubleOsc.prepare (spec);

    if (isFirstRun)
    {
        setLFOType (static_cast<LFOType> (type->getIndex()), true);
        isFirstRun = false;
    }
}

//==============================================================================
template<typename FloatType>
void LFOProcessor::process (dsp::Oscillator<FloatType>& osc,
                            juce::AudioBuffer<FloatType>& multer,
                            juce::AudioBuffer<FloatType>& buffer)
{
    {
        const auto v = getFrequencyHz();
        floatOsc.setFrequency ((float) v);
        doubleOsc.setFrequency (v);
    }

    if (isMultiplying)
    {
        multer.setSize (buffer.getNumChannels(), buffer.getNumSamples(), false, true, true);
        multer.clear();

        dsp::AudioBlock<FloatType> abMulter (multer);

        {
            dsp::ProcessContextReplacing<FloatType> context (abMulter);
            context.isBypassed = isBypassed();
            osc.process (context);
        }

        dsp::AudioBlock<FloatType> (buffer)
            .multiplyBy (abMulter);
    }
    else
    {
        dsp::AudioBlock<FloatType> audioBlock (buffer);
        dsp::ProcessContextReplacing<FloatType> context (audioBlock);
        context.isBypassed = isBypassed();
        osc.process (context);
    }
}

void LFOProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)     { process (floatOsc, floatMulter, buffer); }
void LFOProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)    { process (doubleOsc, doubleMulter, buffer); }
