#if 0

template <typename ResamplerType>
ResamplingProcessor::ResamplingProcessor() :
    realtime (new ResamplerType()),
    offline (new ResamplerType())
{
    static_assert (std::is_base_of<Resampler, ResamplerType>::value, "Class must derive from Resampler");
}

void ResamplingProcessor::setResamplers (Resampler* realtimeResampler, Resampler* offlineResampler)
{
    const ScopedLock sl (getCallbackLock());
    if (realtime.get() != realtimeResampler || offline.get() != offlineResampler)
    {
        realtime.reset (realtimeResampler);
        offline.reset (offlineResampler);
    }

    jassert (realtime != nullptr);
}

void ResamplingProcessor::setRatio (double newRatio)
{
    ratio = std::clamp (newRatio, 0.00001, 5.0);
}

void ResamplingProcessor::setRatio (double sourceRate, double destinationRate)
{
    if (sourceRate > 0.0 && destinationRate > 0.0)
        setRatio (sourceRate / destinationRate);
}

void ResamplingProcessor::prepareToPlay (double newSampleRate, int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);

    const ScopedLock sl (getCallbackLock());

    const int numChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels());

    jassert (realtime != nullptr);
    realtime->prepare (numChans, estimatedSamplesPerBlock, newSampleRate);

    if (offline != nullptr)
        offline->prepare (numChans, estimatedSamplesPerBlock, newSampleRate);

    result.setSize (numChans, estimatedSamplesPerBlock, false, true, true);
}

void ResamplingProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    const int numSamples = buffer.getNumSamples();
    const auto r = getRatio();

    if (isBypassed() || numSamples <= 0 || r == 1.0000000000)
        return;

    const ScopedLock sl (getCallbackLock());

    auto* resamplerToUse = realtime.get();

    if (isNonRealtime() && offline != nullptr)
        resamplerToUse = offline.get();

    jassert (resamplerToUse != nullptr);
    resamplerToUse->setRatio (r);

    const auto newSize = (int) jmax (1.0, (double) numSamples / r);
    result.setSize (buffer.getNumChannels(), newSize, false, false, true);
    result.clear();

    resamplerToUse->process (buffer, result, r);
}

#endif // 
