StereoWidthProcessor::StereoWidthProcessor() :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto vp = std::make_unique<AudioParameterFloat> (ParameterID (getIdentifier().toString(), 1), getName(),
                                                     NormalisableRange<float> (minimumValue, maximumValue),
                                                     defaultValue,
                                                     AudioParameterFloatAttributes ().withLabel (getName()));
    widthParameter = vp.get();
    widthParameter->addListener (this);

    layout.add (std::move (vp));

    setWidth (defaultValue);
    parameterValueChanged (0, defaultValue);

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

    if (! value.isSmoothing()
        && capproximatelyEqual (static_cast<FloatType> (value.getTargetValue()),
                                static_cast<FloatType> (fullFieldValue)))
    {
        return; // Nothing to do.
    }

    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getWritePointer (1);

    for (int i = 0; i < numSamples; ++i)
    {
        constexpr auto half = FloatType (0.5);

        const auto w    = value.getNextValue();
        const auto L    = left[i];
        const auto R    = right[i];
        const auto mid  = (L + R) * half;
        const auto side = (L - R) * half;

        left[i]  = mid + w * side;
        right[i] = mid - w * side;

        // Avoid summed clipping:
        if (w > fullFieldValue)
        {
            const auto scale = fullFieldValue / w;
            left[i]  *= scale;
            right[i] *= scale;
        }
    }
}
