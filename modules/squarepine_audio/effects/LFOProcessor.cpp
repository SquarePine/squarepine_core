//==============================================================================
class LFOProcessor::TypeParameter final : public AudioParameterChoice
{
public:
    TypeParameter() :
        AudioParameterChoice ("type", TRANS ("Type"), getChoices(),
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
LFOProcessor::LFOProcessor() :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto tp = std::make_unique<TypeParameter>();
    type = tp.get();
    layout.add (std::move (tp));

    auto pf = std::make_unique<AudioParameterFloat> ("frequency", "Frequency", 0.1f, 10.0f, 0.5f);
    frequency = pf.get();
    layout.add (std::move (pf));

    resetAPVTSWithLayout (std::move (layout));
}

//==============================================================================
template<typename FloatType>
void LFOProcessor::setLFOType (dsp::Oscillator<FloatType>& osc, LFOType lfoType)
{
    std::function<FloatType (FloatType)> func;

    switch (lfoType)
    {
        case LFOType::sine:     func = (FloatType (*) (FloatType)) &std::sin; break;
        case LFOType::cosine:   func = (FloatType (*) (FloatType)) &std::cos; break;
        case LFOType::tangent:  func = (FloatType (*) (FloatType)) &std::tan; break;
        case LFOType::triangle: func = oscillatorFunctions::triangle<FloatType>; break;
        case LFOType::ramp:     func = oscillatorFunctions::ramp<FloatType>; break;
        case LFOType::sawtooth: func = oscillatorFunctions::saw<FloatType>; break;
        case LFOType::square:   func = oscillatorFunctions::square<FloatType>; break;

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

void LFOProcessor::setLFOType (LFOType lfoType)
{
    setLFOType (lfoType, true);
}

void LFOProcessor::setFrequencyHz (const double newFrequency)
{
    *frequency = (float) newFrequency;

    floatOsc.setFrequency ((float) newFrequency);
    doubleOsc.setFrequency (newFrequency);
}

void LFOProcessor::setFrequencyFromMidiNote (const int midiNote)
{
    setFrequencyHz (MidiMessage::getMidiNoteInHertz (midiNote));
}

double LFOProcessor::getFrequency() const               { return frequency->get(); }
LFOProcessor::LFOType LFOProcessor::getLFOType() const  { return (LFOType) type->getIndex(); }

//==============================================================================
void LFOProcessor::prepareToPlay (const double newSampleRate, const int samplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, samplesPerBlock);

    const auto numChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels());

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
        isFirstRun = false;

        setFrequencyHz (*frequency);
        setLFOType (static_cast<LFOType> (type->getIndex()), true);
    }
}

//==============================================================================
template<typename FloatType>
void LFOProcessor::process (dsp::Oscillator<FloatType>& osc,
                            juce::AudioBuffer<FloatType>& multer,
                            juce::AudioBuffer<FloatType>& buffer)
{
    if (isBypassed())
        return;

    setFrequencyHz (getFrequency());

    multer.setSize (buffer.getNumChannels(), buffer.getNumSamples(), false, true, true);
    multer.clear();

    dsp::AudioBlock<FloatType> abMulter (multer);

    {
        dsp::ProcessContextReplacing<FloatType> context (abMulter);
        osc.process (context);
    }

    dsp::AudioBlock<FloatType> (buffer)
        .multiplyBy (abMulter);
}

void LFOProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)     { process (floatOsc, floatMulter, buffer); }
void LFOProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)    { process (doubleOsc, doubleMulter, buffer); }
