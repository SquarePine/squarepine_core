SimpleNoiseGateProcessor::SimpleNoiseGateProcessor() :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto addFloatParam = [&] (StringRef id, StringRef name, float start, float end, float defaultValue)
    {
        auto newParam = std::make_unique<AudioParameterFloat> (id, name, start, end, defaultValue);
        auto* np = newParam.get();
        layout.add (std::move (newParam));
        return np;
    };

    threshold   = addFloatParam ("threshold",   NEEDS_TRANS ("Threshold"),  1.0f, 99.0f, 50.0f);
    ratio       = addFloatParam ("ratio",       NEEDS_TRANS ("Ratio"),      1.0f, 99.0f, 50.0f);
    attack      = addFloatParam ("attack",      NEEDS_TRANS ("Attack"),     0.0f, 1.0f, 1.0f);
    release     = addFloatParam ("release",     NEEDS_TRANS ("Release"),    0.0f, 99.0f, 50.0f);

    resetAPVTSWithLayout (std::move (layout));
}

//==============================================================================
void SimpleNoiseGateProcessor::setThreshold (float v)   { threshold->operator= (v); }
float SimpleNoiseGateProcessor::getThreshold() const    { return threshold->get(); }
void SimpleNoiseGateProcessor::setRatio (float v)       { ratio->operator= (v); }
float SimpleNoiseGateProcessor::getRatio() const        { return ratio->get(); }
void SimpleNoiseGateProcessor::setAttack (float v)      { attack->operator= (v); }
float SimpleNoiseGateProcessor::getAttack() const       { return attack->get(); }
void SimpleNoiseGateProcessor::setRelease (float v)     { release->operator= (v); }
float SimpleNoiseGateProcessor::getRelease() const      { return release->get(); }

//==============================================================================
void SimpleNoiseGateProcessor::prepareToPlay (double sampleRate, int bufferSize)
{
    setRateAndBufferSizeDetails (sampleRate, bufferSize);

    const dsp::ProcessSpec spec =
    {
        sampleRate,
        (uint32) bufferSize,
        (uint32) std::max (getTotalNumInputChannels(), getTotalNumOutputChannels())
    };

    floatNoiseGate.prepare (spec);
    doubleNoiseGate.prepare (spec);
}

void SimpleNoiseGateProcessor::releaseResources()
{
    floatNoiseGate.reset();
    doubleNoiseGate.reset();
}

//==============================================================================
void SimpleNoiseGateProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatNoiseGate);
}

void SimpleNoiseGateProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleNoiseGate);
}

template<typename FloatType>
void SimpleNoiseGateProcessor::process (juce::AudioBuffer<FloatType>& buffer,
                                        dsp::NoiseGate<FloatType>& compressor)
{
    const auto t = threshold->get();
    const auto ra = ratio->get();
    const auto a = attack->get();
    const auto re = release->get();

    floatNoiseGate.setThreshold (t);
    floatNoiseGate.setRatio (ra);
    floatNoiseGate.setAttack (a);
    floatNoiseGate.setRelease (re);

    doubleNoiseGate.setThreshold ((double) t);
    doubleNoiseGate.setRatio ((double) ra);
    doubleNoiseGate.setAttack ((double) a);
    doubleNoiseGate.setRelease ((double) re);

    dsp::AudioBlock<FloatType> block (buffer);
    dsp::ProcessContextReplacing<FloatType> context (block);
    context.isBypassed = isBypassed();
    compressor.process (context);
}
