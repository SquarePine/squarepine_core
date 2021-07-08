VolumeProcessor::VolumeProcessor() :
    VolumeProcessor (NormalisableRange<float> (0.0f, maximumVolume))
{}

VolumeProcessor::VolumeProcessor (NormalisableRange<float> volumeRange) :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto vp = std::make_unique<AudioParameterFloat> (getIdentifier().toString(),
                                                     getName(),
                                                     volumeRange,
                                                     1.0f,
                                                     getName(),
                                                     AudioProcessorParameter::outputGain,
                                                     [] (float value, int) -> String
                                                     {
                                                        if (approximatelyEqual (value, 1.0f))
                                                            return "0 dB";

                                                        return Decibels::toString (Decibels::gainToDecibels (value));
                                                     });

    volumeParameter = vp.get();
    volumeParameter->addListener (this);

    layout.add (std::move (vp));

    DBG (volumeParameter->getCurrentValueAsText());

    setVolume (getVolume());

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}

//==============================================================================
void VolumeProcessor::setVolume (float v)
{
    v = std::clamp (v, 0.0f, maximumVolume);
    volumeParameter->operator= (v);
}

float VolumeProcessor::getVolume() const noexcept
{
    return volumeParameter->get();
}

//==============================================================================
void VolumeProcessor::parameterValueChanged (int, float newValue)
{
    newValue = getVolume(); // Easier to do this than to use the normalised value...

    const ScopedLock sl (getCallbackLock());
    floatGain.setTargetValue (newValue);
    doubleGain.setTargetValue ((double) newValue);
}

void VolumeProcessor::parameterGestureChanged (int, bool)
{
}

//==============================================================================
void VolumeProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, samplesPerBlock);

    const ScopedLock sl (getCallbackLock());
    floatGain.reset (sampleRate, 0.001);
    doubleGain.reset (sampleRate, 0.001);
}

//==============================================================================
void VolumeProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatGain);
}

void VolumeProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleGain);
}

template<typename FloatType>
void VolumeProcessor::process (juce::AudioBuffer<FloatType>& buffer, 
                               LinearSmoothedValue<FloatType>& value)
{
    if (isBypassed())
        return;

    const ScopedLock sl (getCallbackLock());
    value.applyGain (buffer, buffer.getNumSamples());
}
