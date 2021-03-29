LFOProcessor::LFOProcessor() :
    lfo (new SineLFO())
{
    addParameter (frequency = new AudioParameterDouble ("frequency", "Frequency", 1.f, 20000.f, 440.f));
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
    if (*frequency == newFrequency)
        return;

    *frequency = newFrequency;

    const ScopedLock sl (getCallbackLock());
    configuration.frequency = frequency->get();

    //@todo if the frequency is set using getParameters(), the phase will not be reset.
    configuration.currentPhase = 0.0;
}

void LFOProcessor::setFrequencyFromMidiNote (const int midiNote)
{
    setFrequency (MidiMessage::getMidiNoteInHertz (midiNote));
}

//==============================================================================
const String LFOProcessor::getName() const
{
    return "LFO";
}

Identifier LFOProcessor::getIdentifier() const
{
    return "LFO";
}

//==============================================================================
void LFOProcessor::prepareToPlay (const double newSampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);

    const ScopedLock sl (getCallbackLock());
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);
    configuration.prepare (newSampleRate, configuration.frequency);
}

bool LFOProcessor::isInstrument() const
{
    return true;
}

void LFOProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
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
