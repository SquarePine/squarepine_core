//==============================================================================
class SimpleDistortionProcessor::AmountParameter : public AudioParameterFloat
{
public:
    AmountParameter() noexcept :
        AudioParameterFloat ("AmountId", TRANS ("Amout"), 0.0f, 1.0f, 1.0f)
    {
    }

    int getNumSteps() const override { return 100; }

    String getLabel() const override
    {
        return "%";
    }

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
    addParameter (amountParam);
}

//==============================================================================
const String SimpleDistortionProcessor::getName() const
{
    return TRANS ("Simple Distortion");
}

Identifier SimpleDistortionProcessor::getIdentifier() const
{
    return "Simple Distortion";
}

//==============================================================================
void SimpleDistortionProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    auto localAmount = 1.0f;

    {
        const ScopedLock sl (getCallbackLock());
        localAmount = amountParam->get();
    }

    DistortionFunctions::performSimple (buffer, localAmount);
}
