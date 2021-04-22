void DitherProcessor::prepareToPlay (const double newSampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);

    dithers.clearQuick (true);

    for (auto i = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels()); --i >= 0;)
        dithers.add (new BasicDither());
}

void DitherProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    if (isBypassed())
        return;

    const auto numSamples = buffer.getNumSamples();

    for (int i = buffer.getNumChannels(); --i >= 0;)
        if (auto* d = dithers[i])
            d->process (buffer.getWritePointer (i), numSamples);
}
