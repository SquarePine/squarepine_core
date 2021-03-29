AudioSourceProcessor::AudioSourceProcessor() :
    info (nullptr, 0, 1),
    intermittentBuffer (2, 1024)
{
}

//==============================================================================
void AudioSourceProcessor::setAudioSource (AudioSource* newSource, const bool takeOwnership)
{
    const ScopedLock lock (getCallbackLock());

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

    const ScopedLock lock (getCallbackLock());

    if (audioSource != nullptr)
        audioSource->prepareToPlay (estimatedSamplesPerBlock, newSampleRate);
}

void AudioSourceProcessor::releaseResources()
{
    const ScopedLock lock (getCallbackLock());

    if (audioSource != nullptr)
        audioSource->releaseResources();
}

void AudioSourceProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    const ScopedLock lock (getCallbackLock());

    if (audioSource != nullptr)
    {
        buffer.clear();
        info.numSamples = buffer.getNumSamples();
        info.buffer = &buffer;

        audioSource->getNextAudioBlock (info);
    }
}
