SimpleEQProcessor::SimpleEQProcessor()
{
    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", createParameterLayout()));
}

//==============================================================================
void SimpleEQProcessor::prepareToPlay (const double newSampleRate, const int bufferSize)
{
    setRateAndBufferSizeDetails (newSampleRate, bufferSize);

    const auto numChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels());

    const ScopedLock sl (getCallbackLock());

    for (auto* f : filters)
        f->prepare (newSampleRate, bufferSize, numChans);
}

void SimpleEQProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)    { process (buffer); }
void SimpleEQProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)   { process (buffer); }
