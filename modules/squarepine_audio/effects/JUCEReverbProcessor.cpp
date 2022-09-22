JUCEReverbProcessor::JUCEReverbProcessor()
{
    AudioProcessor::addParameter (roomSize = new AudioParameterFloat ("roomSize", "Room Size", 0.0f, 1.0f, 0.5f));
    AudioProcessor::addParameter (damping = new AudioParameterFloat ("damping", "Damping", 0.0f, 1.0f, 0.5f));
    AudioProcessor::addParameter (wetLevel = new AudioParameterFloat ("wetLevel", "Wet Level", 0.0f, 1.0f, 0.33f));
    AudioProcessor::addParameter (dryLevel = new AudioParameterFloat ("dryLevel", "Dry Level", 0.0f, 1.0f, 0.4f));
    AudioProcessor::addParameter (width = new AudioParameterFloat ("width", "Width", 0.0f, 1.0f, 1.0f));
    AudioProcessor::addParameter (freezeMode = new AudioParameterBool ("freezeMode", "Freeze Mode", false));
}

//==============================================================================
void JUCEReverbProcessor::prepareToPlay (const double newSampleRate, const int bufferSize)
{
    setRateAndBufferSizeDetails (newSampleRate, bufferSize);

    const ScopedLock sl (getCallbackLock());
    reverb.reset();
    reverb.setSampleRate (newSampleRate);
}

void JUCEReverbProcessor::releaseResources()
{
    const ScopedLock sl (getCallbackLock());
    reverb.reset();
}

void JUCEReverbProcessor::updateReverbParameters()
{
    Reverb::Parameters localParams;

    localParams.roomSize = roomSize->get();
    localParams.damping = damping->get();
    localParams.wetLevel = wetLevel->get();
    localParams.dryLevel = dryLevel->get();
    localParams.width = width->get();
    localParams.freezeMode = freezeMode->get();

    {
        const ScopedLock sl (getCallbackLock());
        reverb.setParameters (localParams);
    }
}

void JUCEReverbProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (isBypassed()
        || buffer.hasBeenCleared()
        || numChannels <= 0
        || numSamples <= 0)
        return;

    updateReverbParameters();

    auto chans = buffer.getArrayOfWritePointers();

    const ScopedLock sl (getCallbackLock());

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
