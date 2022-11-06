AudioSourceProcessor::AudioSourceProcessor() :
    info (nullptr, 0, 1),
    intermittentBuffer (2, 1024)
{
}

//==============================================================================
void AudioSourceProcessor::setAudioSource (AudioSource* newSource, const bool takeOwnership)
{
    if (audioSource != newSource)
        audioSource.set (newSource, takeOwnership);
}

//==============================================================================
Identifier AudioSourceProcessor::getIdentifier() const
{
    return "AudioSourceProcessor";
}

void AudioSourceProcessor::prepareToPlay (const double newSampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);

    if (audioSource != nullptr)
        audioSource->prepareToPlay (estimatedSamplesPerBlock, newSampleRate);
}

void AudioSourceProcessor::releaseResources()
{
    if (audioSource != nullptr)
        audioSource->releaseResources();
}

void AudioSourceProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    if (audioSource != nullptr)
    {
        buffer.clear();
        info.numSamples = buffer.getNumSamples();
        info.buffer = &buffer;

        audioSource->getNextAudioBlock (info);
    }
}
