GainProcessor::GainProcessor (const String& parameterName,
                              NormalisableRange<float> gainRange) :
    InternalProcessor (false),
    name (parameterName)
{
    auto layout = createDefaultParameterLayout();

    auto vp = std::make_unique<GainParameter> (getIdentifier().toString(), getName(),
                                               gainRange, 1.0f, getName(),
                                               AudioProcessorParameter::outputGain,
                                               [] (float value, int) -> String
                                               {
                                                    if (approximatelyEqual (value, 1.0f))
                                                        return "0 dB";

                                                    return Decibels::toString (Decibels::gainToDecibels (value));
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

    const ScopedLock sl (getCallbackLock());
    floatGain.setTargetValue (newValue);
    doubleGain.setTargetValue ((double) newValue);
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
