LFOProcessor::LFOProcessor() :
    lfo (new SineLFO())
{
    addParameter (frequency = new AudioParameterFloat ("frequency", "Frequency", 1.f, 20000.f, 440.f));
}

//==============================================================================
void LFOProcessor::setLFOType (LFO* const newLfo)
{
    jassert (newLfo != nullptr);

    const ScopedLock sl (getCallbackLock());

    if (lfo.get() != newLfo)
    {
        lfo.reset (newLfo);
        prepareToPlay (getSampleRate(), getBlockSize());
    }
}

void LFOProcessor::setFrequency (const double newFrequency)
{
    const auto newF = (float) newFrequency;
    if (approximatelyEqual (frequency->get(), newF))
        return;

    *frequency = newF;

    const ScopedLock sl (getCallbackLock());
    configuration.frequency = newF;

    //@todo if the frequency is set using getParameters(), the phase will not be reset.
    configuration.currentPhase = 0.0;
}

void LFOProcessor::setFrequencyFromMidiNote (const int midiNote)
{
    setFrequency (MidiMessage::getMidiNoteInHertz (midiNote));
}

//==============================================================================
void LFOProcessor::prepareToPlay (const double newSampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);

    const ScopedLock sl (getCallbackLock());
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);
    configuration.prepare (newSampleRate, configuration.frequency);
}

void LFOProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    MidiMessage result;
    for (const MidiMessageMetadata metadata : midiMessages)
    {
        result = metadata.getMessage();
        if (result.isNoteOn (true))
            break;
    }

    const ScopedLock sl (getCallbackLock());

    if (lfo != nullptr && result.isNoteOn())
    {
        setFrequencyFromMidiNote (result.getNoteNumber());
        configuration.frequency = frequency->get();
        configuration.currentPhase = lfo->process (buffer, configuration);
    }
}
