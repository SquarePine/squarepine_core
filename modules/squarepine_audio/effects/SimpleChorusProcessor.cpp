SimpleChorusProcessor::SimpleChorusProcessor() :
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

    rate        = addFloatParam ("rate",        NEEDS_TRANS ("Rate"),           1.0f, 99.0f, 3.0f);
    depth       = addFloatParam ("depth",       NEEDS_TRANS ("Depth"),          0.0f, 1.0f, 0.1f);
    centreDelay = addFloatParam ("centreDelay", NEEDS_TRANS ("Centre Delay"),   0.0f, 99.0f, 50.0f);
    feedback    = addFloatParam ("feedback",    NEEDS_TRANS ("Feedback"),       -1.0f, 1.0f, 0.25f);
    mix         = addFloatParam ("mix",         NEEDS_TRANS ("Mix"),            0.0f, 1.0f, 1.0f);

    resetAPVTSWithLayout (std::move (layout));
}

//==============================================================================
void SimpleChorusProcessor::setRate (float v)           { rate->operator= (v); }
float SimpleChorusProcessor::getRate() const            { return rate->get(); }
void SimpleChorusProcessor::setDepth (float v)          { depth->operator= (v); }
float SimpleChorusProcessor::getDepth() const           { return depth->get(); }
void SimpleChorusProcessor::setCentreDelay (float v)    { centreDelay->operator= (v); }
float SimpleChorusProcessor::getCentreDelay() const     { return centreDelay->get(); }
void SimpleChorusProcessor::setFeedback (float v)       { feedback->operator= (v); }
float SimpleChorusProcessor::getFeedback() const        { return feedback->get(); }
void SimpleChorusProcessor::setMix (float v)            { mix->operator= (v); }
float SimpleChorusProcessor::getMix() const             { return mix->get(); }

//==============================================================================
void SimpleChorusProcessor::prepareToPlay (double sampleRate, int bufferSize)
{
    setRateAndBufferSizeDetails (sampleRate, bufferSize);

    const dsp::ProcessSpec spec =
    {
        sampleRate,
        (uint32) bufferSize,
        (uint32) jmax (getTotalNumInputChannels(), getTotalNumOutputChannels())
    };

    floatChorus.prepare (spec);
    doubleChorus.prepare (spec);
}

void SimpleChorusProcessor::releaseResources()
{
    floatChorus.reset();
    doubleChorus.reset();
}

//==============================================================================
void SimpleChorusProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatChorus);
}

void SimpleChorusProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleChorus);
}

template<typename FloatType>
void SimpleChorusProcessor::process (juce::AudioBuffer<FloatType>& buffer, dsp::Chorus<FloatType>& chorus)
{
    const auto r = rate->get();
    const auto d = depth->get();
    const auto f = feedback->get();
    const auto cd = centreDelay->get();
    const auto m = mix->get();

    floatChorus.setRate (r);
    floatChorus.setDepth (d);
    floatChorus.setFeedback (f);
    floatChorus.setCentreDelay (cd);
    floatChorus.setMix (m);

    doubleChorus.setRate ((double) r);
    doubleChorus.setDepth ((double) d);
    doubleChorus.setFeedback ((double) f);
    doubleChorus.setCentreDelay ((double) cd);
    doubleChorus.setMix ((double) m);

    dsp::AudioBlock<FloatType> block (buffer);
    dsp::ProcessContextReplacing<FloatType> context (block);
    context.isBypassed = isBypassed();
    chorus.process (context);
}
