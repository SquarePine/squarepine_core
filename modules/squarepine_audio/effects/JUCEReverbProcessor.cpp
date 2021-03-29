JUCEReverbProcessor::JUCEReverbProcessor()
{
    addParameter (roomSize = new AudioParameterFloat ("roomSize", "Room Size", 0.f, 1.f, 0.5f));
    addParameter (damping = new AudioParameterFloat ("damping", "Damping", 0.f, 1.f, 0.5f));
    addParameter (wetLevel = new AudioParameterFloat ("wetLevel", "Wet Level", 0.f, 1.f, 0.33f));
    addParameter (dryLevel = new AudioParameterFloat ("dryLevel", "Dry Level", 0.f, 1.f, 0.4f));
    addParameter (width = new AudioParameterFloat ("width", "Width", 0.f, 1.f, 1.0f));
    addParameter (freezeMode = new AudioParameterBool ("freezeMode", "Freeze Mode", false));
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

void JUCEReverbProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{

    if (! buffer.hasBeenCleared())
    {
        updateReverbParameters();

        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        float** chans = buffer.getArrayOfWritePointers();

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
}

//==============================================================================

//@todo
//int JUCEReverbProcessor::getParameterNumSteps (const int parameterIndex)
//{
//    switch (parameterIndex)
//    {
//        case 5: return 2; //Freeze mode
//
//        default:
//        break;
//    };
//
//    return AudioProcessor::getDefaultNumParameterSteps();
//}

const String JUCEReverbProcessor::getName() const
{
    return NEEDS_TRANS ("Simple Reverb");
}

Identifier JUCEReverbProcessor::getIdentifier() const
{
    return "SimpleReverb";
}
