ADSRProcessor::ADSRProcessor()
{
    addParameter (new AudioParameterFloat ("attack", "Attack", 0.0f, 1.0f, 0.1f));
    addParameter (new AudioParameterFloat ("decay", "Decay", 0.0f, 1.0f, 0.1f));
    addParameter (new AudioParameterFloat ("sustain", "Sustain", 0.0f, 1.0f, 1.0f));
    addParameter (new AudioParameterFloat ("release", "Release", 0.0f, 1.0f, 0.1f));
}

//==============================================================================
void ADSRProcessor::prepareToPlay (double sampleRate, int bufferSize)
{
    setPlayConfigDetails (2, 2, sampleRate, bufferSize);

    const ScopedLock sl (getCallbackLock());
    adsr.setSampleRate (sampleRate);
    adsr.reset();
}

void ADSRProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)  { process (buffer); }
void ADSRProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&) { process (buffer); }
