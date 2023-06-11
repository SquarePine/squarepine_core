ADSRProcessor::ADSRProcessor() :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto addFloatParam = [&] (StringRef id, StringRef name, float defaultValue)
    {
        auto apf = std::make_unique<AudioParameterFloat> (id, name, 0.0f, 1.0f, defaultValue);
        layout.add (std::move (apf));
    };

    addFloatParam ("attack",     NEEDS_TRANS ("Attack"),    0.1f);
    addFloatParam ("decay",      NEEDS_TRANS ("Decay"),     0.1f);
    addFloatParam ("sustain",    NEEDS_TRANS ("Sustain"),   1.0f);
    addFloatParam ("release",    NEEDS_TRANS ("Release"),   0.1f);

    resetAPVTSWithLayout (std::move (layout));
}

//==============================================================================
void ADSRProcessor::prepareToPlay (double sampleRate, int bufferSize)
{
    setPlayConfigDetails (2, 2, sampleRate, bufferSize);

    adsr.setSampleRate (sampleRate);
    adsr.reset();
}

void ADSRProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiBuff)   { process (buffer, midiBuff); }
void ADSRProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiBuff)  { process (buffer, midiBuff); }
