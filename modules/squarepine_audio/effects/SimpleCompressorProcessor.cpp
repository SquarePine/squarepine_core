SimpleCompressorProcessor::SimpleCompressorProcessor() :
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

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}

//==============================================================================
void SimpleCompressorProcessor::setThreshold (float v)  { threshold->operator= (v); }
float SimpleCompressorProcessor::getThreshold() const   { return threshold->get(); }
void SimpleCompressorProcessor::setRatio (float v)      { ratio->operator= (v); }
float SimpleCompressorProcessor::getRatio() const       { return ratio->get(); }
void SimpleCompressorProcessor::setAttack (float v)     { attack->operator= (v); }
float SimpleCompressorProcessor::getAttack() const      { return attack->get(); }
void SimpleCompressorProcessor::setRelease (float v)    { release->operator= (v); }
float SimpleCompressorProcessor::getRelease() const     { return release->get(); }

//==============================================================================
void SimpleCompressorProcessor::prepareToPlay (double sampleRate, int bufferSize)
{
    setRateAndBufferSizeDetails (sampleRate, bufferSize);

    const dsp::ProcessSpec spec =
    {
        sampleRate,
        (uint32) bufferSize,
        (uint32) jmax (getTotalNumInputChannels(), getTotalNumOutputChannels())
    };

    floatCompressor.prepare (spec);
    doubleCompressor.prepare (spec);
}

void SimpleCompressorProcessor::releaseResources()
{
    floatCompressor.reset();
    doubleCompressor.reset();
}

//==============================================================================
void SimpleCompressorProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatCompressor);
}

void SimpleCompressorProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleCompressor);
}

template<typename FloatType>
void SimpleCompressorProcessor::process (juce::AudioBuffer<FloatType>& buffer,
                                         dsp::Compressor<FloatType>& compressor)
{
    const auto t = threshold->get();
    const auto ra = ratio->get();
    const auto a = attack->get();
    const auto re = release->get();

    floatCompressor.setThreshold (t);
    floatCompressor.setRatio (ra);
    floatCompressor.setAttack (a);
    floatCompressor.setRelease (re);

    doubleCompressor.setThreshold ((double) t);
    doubleCompressor.setRatio ((double) ra);
    doubleCompressor.setAttack ((double) a);
    doubleCompressor.setRelease ((double) re);

    dsp::AudioBlock<FloatType> block (buffer);
    dsp::ProcessContextReplacing<FloatType> context (block);
    context.isBypassed = isBypassed();
    compressor.process (context);
}
