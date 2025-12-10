//==============================================================================
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

    auto vp = std::make_unique<AudioParameterFloat> (ParameterID (getIdentifier().toString(), 1), getName(), gainRange, 1.0f,
                                                     AudioParameterFloatAttributes()
                                                        .withCategory (AudioParameterFloatAttributes::Category::outputGain)
                                                        .withLabel (getName())
                                                        .withStringFromValueFunction (toStringFromGainValue));
    gainParameter = vp.get();
    gainParameter->addListener (this);
    layout.add (std::move (vp));
    setGain (getGain());
    resetAPVTSWithLayout (std::move (layout));
}

//==============================================================================
void GainProcessor::setGain (float v)
{
    v = std::clamp (v, getMinimumGain(), getMaximumGain());
    gainParameter->operator= (v);
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
    if (! isBypassed())
        value.applyGain (buffer, buffer.getNumSamples());
}
