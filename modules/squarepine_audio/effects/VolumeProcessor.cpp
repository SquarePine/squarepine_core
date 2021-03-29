//==============================================================================
VolumeProcessor::VolumeProcessor()
{
    volumeParameter = new AudioParameterFloat (volumeId.toString(), getName(),
                                               NormalisableRange<float> (0.0f, maximumVolume, 0.001f),
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
const String VolumeProcessor::getName() const
{
    return TRANS ("Volume");
}

void VolumeProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, samplesPerBlock);

    currentGain = volumeParameter->get();
}

//==============================================================================
void VolumeProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    if (currentGain != volumeParameter->get())
    {
        buffer.applyGainRamp (0, buffer.getNumSamples(), currentGain, volumeParameter->get());
        currentGain = volumeParameter->get();
    }
    else
    {
        buffer.applyGain (currentGain);
    }
}
