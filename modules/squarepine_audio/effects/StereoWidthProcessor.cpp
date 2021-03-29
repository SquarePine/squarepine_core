//==============================================================================
class StereoWidthProcessor::WidthParameter final : public AudioParameterFloat
{
public:
    WidthParameter() :
        AudioParameterFloat ("stereoWidthId", TRANS ("Width"), {}, StereoWidthProcessor::getNormal())
    {
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WidthParameter)
};

//==============================================================================
StereoWidthProcessor::StereoWidthProcessor() :
    widthParameter (new WidthParameter())
{
    addParameter (widthParameter);
}

//==============================================================================
void StereoWidthProcessor::setWidth (const float newWidth)
{
    widthParameter->juce::AudioParameterFloat::operator= (newWidth);
}

float StereoWidthProcessor::getWidth() const
{
    return widthParameter->get();
}

//==============================================================================
void StereoWidthProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    const auto numSamples = buffer.getNumSamples();

    if (numSamples <= 0 || isBypassed() || buffer.getNumChannels() < 2)
        return;

    auto* leftChannel = buffer.getWritePointer (0);
    auto* rightChannel = buffer.getWritePointer (1);

    auto localWidth = 0.0f;

    {
        const ScopedLock sl (getCallbackLock());
        localWidth = widthParameter->get() * 2.0f;
    }

    const auto coeffM = 1.0f / jmax (1.0f + localWidth, 2.0f);
    const auto coeffS = localWidth * coeffM;

    for (int i = numSamples; --i >= 0;)
    {
        const auto sampleLeft = leftChannel[i];
        const auto sampleRight = rightChannel[i];

        const auto M = coeffM * (sampleLeft + sampleRight);
        const auto S = coeffS * (sampleRight - sampleLeft);

        leftChannel[i] = M - S;
        rightChannel[i] = M + S;
    }
}
