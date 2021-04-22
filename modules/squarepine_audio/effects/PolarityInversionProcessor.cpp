//==============================================================================
class PolarityInversionProcessor::InvertParameter final : public AudioParameterBool
{
public:
    InvertParameter() :
        AudioParameterBool ("polarityInversionId", TRANS ("Invert Polarity"), false)
    {
    }

    String getText (float v, int maximumStringLength) const override
    {
        return (v >= 0.5f ? TRANS ("Active") : TRANS ("Normal"))
               .substring (0, maximumStringLength);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InvertParameter)
};

//==============================================================================
PolarityInversionProcessor::PolarityInversionProcessor() :
    invertParameter (new InvertParameter())
{
    addParameter (invertParameter);
}

//==============================================================================
void PolarityInversionProcessor::setActive (bool shouldBeActive)
{
    const ScopedLock sl (getCallbackLock());        
    invertParameter->juce::AudioParameterBool::operator= (shouldBeActive);
}

bool PolarityInversionProcessor::isActive() const
{
    if (invertParameter != nullptr)
        return invertParameter->get();

    return false;
}

//==============================================================================
void PolarityInversionProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiBuffer)
{
    process (buffer, midiBuffer);
}

void PolarityInversionProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiBuffer)
{
    process (buffer, midiBuffer);
}
