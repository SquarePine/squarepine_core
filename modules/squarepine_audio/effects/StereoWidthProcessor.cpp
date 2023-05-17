StereoWidthProcessor::StereoWidthProcessor() :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto vp = std::make_unique<AudioParameterFloat> (getIdentifier().toString(), getName(),
                                                     NormalisableRange<float> (minimumValue, maximumValue),
                                                     defaultValue,
                                                     AudioParameterFloatAttributes ().withLabel (getName()));
    widthParameter = vp.get();
    widthParameter->addListener (this);

    layout.add (std::move (vp));

    setWidth (getWidth());

    resetAPVTSWithLayout (std::move (layout));
}

//==============================================================================
void StereoWidthProcessor::setWidth (float v)
{
    v = std::clamp (v, minimumValue, maximumValue);
    widthParameter->operator= (v);
}

float StereoWidthProcessor::getWidth() const noexcept
{
    return widthParameter->get();
}

void StereoWidthProcessor::parameterValueChanged (int, float newValue)
{
    newValue = getWidth(); // Easier to do this than to use the normalised value...

    floatWidth.setTargetValue (newValue);
    doubleWidth.setTargetValue ((double) newValue);
}

//==============================================================================
void StereoWidthProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, samplesPerBlock);

    floatWidth.reset (sampleRate, 0.001);
    doubleWidth.reset (sampleRate, 0.001);
}

void StereoWidthProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatWidth);
}

void StereoWidthProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleWidth);
}

template<typename FloatType>
void StereoWidthProcessor::process (juce::AudioBuffer<FloatType>& buffer, 
                                    LinearSmoothedValue<FloatType>& value)
{
    if (isBypassed())
        return;

    const auto numSamples = buffer.getNumSamples();
    if (numSamples <= 0
        || getSampleRate() <= 0.0)
    {
        buffer.clear();
        return;
    }

    constexpr auto one = static_cast<FloatType> (1);
    constexpr auto two = static_cast<FloatType> (2);

    const auto localWidth = value.getNextValue() * two;
    const auto coeffM = one / jmax (one + localWidth, two);
    const auto coeffS = localWidth * coeffM;

    auto* leftChannel = buffer.getWritePointer (0);
    auto* rightChannel = buffer.getWritePointer (1);

    for (int i = 0; i < numSamples; ++i)
    {
        const auto sampleLeft = leftChannel[i];
        const auto sampleRight = rightChannel[i];

        const auto mid = coeffM * (sampleLeft + sampleRight);
        const auto side = coeffS * (sampleRight - sampleLeft);

        leftChannel[i] = mid - side;
        rightChannel[i] = mid + side;
    }
}
