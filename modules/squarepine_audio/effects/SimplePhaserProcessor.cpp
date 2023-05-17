SimplePhaserProcessor::SimplePhaserProcessor() :
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

    rate            = addFloatParam ("roomSize",        NEEDS_TRANS ("Rate"),               1.0f, 99.0f, 10.0f);
    depth           = addFloatParam ("depth",           NEEDS_TRANS ("Depth"),              0.0f, 1.0f, 1.0f);
    centreFrequency = addFloatParam ("centreFrequency", NEEDS_TRANS ("Centre Frequency"),   1.0f, 10000.0f, 50.0f);
    feedback        = addFloatParam ("feedback",        NEEDS_TRANS ("Feedback"),           -1.0f, 1.0f, -0.5f);
    mix             = addFloatParam ("mix",             NEEDS_TRANS ("Mix"),                0.0f, 1.0f, 1.0f);

    resetAPVTSWithLayout (std::move (layout));
}

//==============================================================================
void SimplePhaserProcessor::setRate (float v)               { rate->operator= (v); }
float SimplePhaserProcessor::getRate() const                { return rate->get(); }
void SimplePhaserProcessor::setDepth (float v)              { depth->operator= (v); }
float SimplePhaserProcessor::getDepth() const               { return depth->get(); }
void SimplePhaserProcessor::setCentreFrequency (float v)    { centreFrequency->operator= (v); }
float SimplePhaserProcessor::getCentreFrequency() const     { return centreFrequency->get(); }
void SimplePhaserProcessor::setFeedback (float v)           { feedback->operator= (v); }
float SimplePhaserProcessor::getFeedback() const            { return feedback->get(); }
void SimplePhaserProcessor::setMix (float v)                { mix->operator= (v); }
float SimplePhaserProcessor::getMix() const                 { return mix->get(); }

//==============================================================================
void SimplePhaserProcessor::prepareToPlay (double sampleRate, int bufferSize)
{
    setRateAndBufferSizeDetails (sampleRate, bufferSize);

    const dsp::ProcessSpec spec =
    {
        sampleRate,
        (uint32) bufferSize,
        (uint32) jmax (getTotalNumInputChannels(), getTotalNumOutputChannels())
    };

    floatPhaser.prepare (spec);
    doublePhaser.prepare (spec);
}

void SimplePhaserProcessor::releaseResources()
{
    floatPhaser.reset();
    doublePhaser.reset();
}

//==============================================================================
void SimplePhaserProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatPhaser);
}

void SimplePhaserProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doublePhaser);
}

template<typename FloatType>
void SimplePhaserProcessor::process (juce::AudioBuffer<FloatType>& buffer, dsp::Phaser<FloatType>& chorus)
{
    const auto r = rate->get();
    const auto d = depth->get();
    const auto f = feedback->get();
    const auto cf = centreFrequency->get();
    const auto m = mix->get();

    floatPhaser.setRate (r);
    floatPhaser.setDepth (d);
    floatPhaser.setFeedback (f);
    floatPhaser.setCentreFrequency (cf);
    floatPhaser.setMix (m);

    doublePhaser.setRate ((double) r);
    doublePhaser.setDepth ((double) d);
    doublePhaser.setFeedback ((double) f);
    doublePhaser.setCentreFrequency ((double) cf);
    doublePhaser.setMix ((double) m);

    dsp::AudioBlock<FloatType> block (buffer);
    dsp::ProcessContextReplacing<FloatType> context (block);
    context.isBypassed = isBypassed();
    chorus.process (context);
}
