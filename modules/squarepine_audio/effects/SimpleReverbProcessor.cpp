SimpleReverbProcessor::SimpleReverbProcessor() :
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

    roomSize  = addFloatParam ("roomSize",  NEEDS_TRANS ("Room Size"),  0.0f, 1.0f, 0.5f);
    damping   = addFloatParam ("damping",   NEEDS_TRANS ("Damping"),    0.0f, 1.0f, 0.5f);
    wetLevel  = addFloatParam ("wetLevel",  NEEDS_TRANS ("Wet Level"),  0.0f, 1.0f, 0.33f);
    dryLevel  = addFloatParam ("dryLevel",  NEEDS_TRANS ("Dry Level"),  0.0f, 1.0f, 0.4f);
    width     = addFloatParam ("width",     NEEDS_TRANS ("Width"),      0.0f, 1.0f, 1.0f);

    auto freezeModeParam = std::make_unique<AudioParameterBool> ("freezeMode", NEEDS_TRANS ("Freeze Mode"), false);
    freezeMode = freezeModeParam.get();
    layout.add (std::move (freezeModeParam));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}

//==============================================================================
void SimpleReverbProcessor::prepareToPlay (const double newSampleRate, const int bufferSize)
{
    setRateAndBufferSizeDetails (newSampleRate, bufferSize);

    reverb.reset();
    reverb.setSampleRate (newSampleRate);
}

void SimpleReverbProcessor::releaseResources()
{
    reverb.reset();
}

void SimpleReverbProcessor::updateReverbParameters()
{
    Reverb::Parameters localParams;

    localParams.roomSize = roomSize->get();
    localParams.damping = damping->get();
    localParams.wetLevel = wetLevel->get();
    localParams.dryLevel = dryLevel->get();
    localParams.width = width->get();
    localParams.freezeMode = freezeMode->get();

    reverb.setParameters (localParams);
}

void SimpleReverbProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (isBypassed()
        || buffer.hasBeenCleared()
        || numSamples <= 0)
        return;

    updateReverbParameters();

    auto chans = buffer.getArrayOfWritePointers();

    switch (numChannels)
    {
        case 1:
            reverb.processMono (chans[0], numSamples);
        break;

        case 2:
            reverb.processStereo (chans[0], chans[1], numSamples);
        break;

        default:
        break;
    }
}
