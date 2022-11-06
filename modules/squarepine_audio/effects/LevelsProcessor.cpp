LevelsProcessor::LevelsProcessor() :
    InternalProcessor (false)
{
}

//==============================================================================
void LevelsProcessor::setMode (MeteringMode newMode)
{
    mode.store (newMode, std::memory_order_relaxed);
}

MeteringMode LevelsProcessor::getMode() const noexcept
{
    return mode.load (std::memory_order_relaxed);
}

//==============================================================================
void LevelsProcessor::getChannelLevels (Array<float>& destData)
{
    getChannelLevels (destData, floatChannelDetails);
}

void LevelsProcessor::getChannelLevels (Array<double>& destData)
{
    getChannelLevels (destData, doubleChannelDetails);
}

//==============================================================================
void LevelsProcessor::prepareToPlay (double newSampleRate, int bufferSize)
{
    setRateAndBufferSizeDetails (newSampleRate, bufferSize);

    const auto numChannels = jmax (2, getTotalNumInputChannels(), getTotalNumOutputChannels());

    floatChannelDetails.prepare (numChannels);
    doubleChannelDetails.prepare (numChannels);
}

//==============================================================================
void LevelsProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatChannelDetails);
}

void LevelsProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleChannelDetails);
}
