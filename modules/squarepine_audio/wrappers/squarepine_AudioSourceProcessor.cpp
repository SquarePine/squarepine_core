AudioSourceProcessor::AudioSourceProcessor() :
    info (nullptr, 0, 1)
{
}

//==============================================================================
void AudioSourceProcessor::setAudioSource (AudioSource* newSource, const bool takeOwnership)
{
    if (audioSource != newSource)
        audioSource.set (newSource, takeOwnership);
}

//==============================================================================
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
    buffer.clear();

    if (isSuspended() || isBypassed())
        return;

    if (audioSource != nullptr)
    {
        info.numSamples = buffer.getNumSamples();
        info.buffer = &buffer;
        audioSource->getNextAudioBlock (info);
    }
}
