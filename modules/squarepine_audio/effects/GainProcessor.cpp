namespace
{
    String toStringFromGainValue (float value, int)
    {
        if (approximatelyEqual (value, 1.0f))
            return "0 dB";

        return Decibels::toString (Decibels::gainToDecibels (value));
    }
}

//==============================================================================
GainProcessor::GainProcessor (NormalisableRange<float> gainRange) :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto vp = std::make_unique<NotifiableAudioParameterFloat> (getIdentifier().toString(),
                                                               getName(),
                                                               gainRange,
                                                               0.0f,
                                                               true, // isAutomatable
                                                               getName(),
                                                               AudioProcessorParameter::outputGain,
                                                               [] (float value, int) -> String
                                                               {
                                                                    if (approximatelyEqual (value, 0.0f))
                                                                        return "0 dB";
        
                                                                    if (approximatelyEqual (value, -96.0f))
                                                                        return "-Inf dB";

                                                                    return Decibels::toString (value);
                                                               });

    gainParameter = vp.get();
    gainParameter->addListener (this);

    layout.add (std::move (vp));

    setGain (getGain());

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}

//==============================================================================
void GainProcessor::setGain (float v)
{
    v = std::clamp (v, getMinimumGain(), getMaximumGain());
    
    gainParameter->juce::AudioParameterFloat::operator= (v);
}

float GainProcessor::getGain() const noexcept
{
    jassert (gainParameter != nullptr);
    return gainParameter->get();
}

float GainProcessor::getMinimumGain() const noexcept
{
    jassert (gainParameter != nullptr);
    return gainParameter->range.start;
}

float GainProcessor::getMaximumGain() const noexcept
{
    jassert (gainParameter != nullptr);
    return gainParameter->range.end;
}

//==============================================================================
void GainProcessor::parameterValueChanged (int, float newValue)
{
    newValue = getGain();

    auto linearGain = std::pow(10.f,newValue/20.f); // convert to linear
    const ScopedLock sl (getCallbackLock());
    floatGain.setTargetValue (linearGain);
    doubleGain.setTargetValue ((double) linearGain);
}

void GainProcessor::parameterGestureChanged (int, bool)
{
}

//==============================================================================
void GainProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, samplesPerBlock);

    const ScopedLock sl (getCallbackLock());
    floatGain.reset (sampleRate, 0.001);
    doubleGain.reset (sampleRate, 0.001);
}

//==============================================================================
void GainProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatGain);
}

void GainProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleGain);
}

template<typename FloatType>
void GainProcessor::process (juce::AudioBuffer<FloatType>& buffer,
                             LinearSmoothedValue<FloatType>& value)
{
    if (isBypassed())
        return;

    const ScopedLock sl (getCallbackLock());
    value.applyGain (buffer, buffer.getNumSamples());
}
