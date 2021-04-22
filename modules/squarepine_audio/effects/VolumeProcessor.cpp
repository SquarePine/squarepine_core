//==============================================================================
VolumeProcessor::VolumeProcessor()
{
    volumeParameter = new AudioParameterFloat (volumeId.toString(), getName(),
                                               NormalisableRange<float> (0.0f, maximumVolume, 0.0001f),
                                               1.0f, getName(), AudioProcessorParameter::outputGain);
    addParameter (volumeParameter);
}

//==============================================================================
void VolumeProcessor::setVolume (float newVolume)
{
    volumeParameter->operator= (newVolume);
}

float VolumeProcessor::getVolume() const
{
    return volumeParameter->get();
}

//==============================================================================
void VolumeProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, samplesPerBlock);

    currentGain = volumeParameter->get();
}

//==============================================================================
void VolumeProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    process (buffer, midiMessages);
}

void VolumeProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiMessages)
{
    process (buffer, midiMessages);
}
