SimpleLimiterProcessor::SimpleLimiterProcessor() :
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
    release     = addFloatParam ("release",     NEEDS_TRANS ("Release"),    0.0f, 99.0f, 50.0f);

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}

//==============================================================================
void SimpleLimiterProcessor::setThreshold (float v)  { threshold->operator= (v); }
float SimpleLimiterProcessor::getThreshold() const   { return threshold->get(); }
void SimpleLimiterProcessor::setRelease (float v)    { release->operator= (v); }
float SimpleLimiterProcessor::getRelease() const     { return release->get(); }

//==============================================================================
void SimpleLimiterProcessor::prepareToPlay (double sampleRate, int bufferSize)
{
    setRateAndBufferSizeDetails (sampleRate, bufferSize);

    const dsp::ProcessSpec spec =
    {
        sampleRate,
        (uint32) bufferSize,
        (uint32) jmax (getTotalNumInputChannels(), getTotalNumOutputChannels())
    };

    floatLimiter.prepare (spec);
    doubleLimiter.prepare (spec);
}

void SimpleLimiterProcessor::releaseResources()
{
    floatLimiter.reset();
    doubleLimiter.reset();
}

//==============================================================================
void SimpleLimiterProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatLimiter);
}

void SimpleLimiterProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleLimiter);
}

template<typename FloatType>
void SimpleLimiterProcessor::process (juce::AudioBuffer<FloatType>& buffer,
                                      dsp::Limiter<FloatType>& limiter)
{
    const auto t = threshold->get();
    const auto re = release->get();

    floatLimiter.setThreshold (t);
    floatLimiter.setRelease (re);

    doubleLimiter.setThreshold ((double) t);
    doubleLimiter.setRelease ((double) re);

    dsp::AudioBlock<FloatType> block (buffer);
    dsp::ProcessContextReplacing<FloatType> context (block);
    context.isBypassed = isBypassed();
    limiter.process (context);
}
