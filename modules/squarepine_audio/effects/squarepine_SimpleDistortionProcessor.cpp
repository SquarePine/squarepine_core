//==============================================================================
class SimpleDistortionProcessor::AmountParameter final : public AudioParameterFloat
{
public:
    AmountParameter() noexcept :
        AudioParameterFloat ("AmountId", TRANS ("Amount"), 0.0f, 1.0f, 0.6f)
    {
    }

    int getNumSteps() const override { return 100; }
    String getLabel() const override { return "%"; }

    String getText (float v, int maximumStringLength) const override
    {
        return String (static_cast<int> (v * 100.0f)).substring (0, maximumStringLength);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmountParameter)
};

//==============================================================================
SimpleDistortionProcessor::SimpleDistortionProcessor() :
    amountParam (new AmountParameter())
{
    AudioProcessor::addParameter (amountParam);
}

//==============================================================================
void SimpleDistortionProcessor::prepareToPlay (double newSampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, samplesPerBlock);
}

template<typename FloatType>
void SimpleDistortionProcessor::process (juce::AudioBuffer<FloatType>& buffer)
{
    if (isBypassed())
        return;

    using DistFuncs = DistortionFunctions<FloatType>;
    const auto d = static_cast<FloatType> (amountParam->get());
    DistFuncs::perform (buffer, DistFuncs::sigmoid, d, static_cast<FloatType> (100));
}

void SimpleDistortionProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)    { process (buffer); }
void SimpleDistortionProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)   { process (buffer); }
