//==============================================================================
class PanProcessor::PanParameter final : public AudioParameterFloat
{
public:
    PanParameter() :
        AudioParameterFloat ("panId", TRANS ("Pan"), fullLeft, fullRight, centre)
    {
    }

    String getLabel() const override { return getGeneralPanPosition(); }

    String getText (float v, int maximumStringLength) const override
    {
        if (maximumStringLength <= 1)
            return getGeneralPanPosition();

        const auto doubleValue = (double) v;
        String l = "Centre";

        if (doubleValue < centre)
            l = String (doubleValue * 100.0, 0) + "% L";
        else if (doubleValue > centre)
            l = String (doubleValue * 100.0, 0) + "% R";

        return l.substring (0, maximumStringLength);
    }

private:
    String getGeneralPanPosition() const
    {
        const auto v = get();

        if (v == centre)        return "C";
        else if (v > centre)    return "R";

        return "L";
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanParameter)
};

//==============================================================================
class PanProcessor::PanRuleParameter final : public AudioParameterChoice
{
public:
    PanRuleParameter() :
        AudioParameterChoice ("panRuleId",
                              TRANS ("Pan Rule"),
                              getChoices(),
                              static_cast<int> (PanProcessor::defaultPannerRule))
    {
    }

private:
    static StringArray getChoices()
    {
        StringArray choices;
        choices.add (TRANS ("Linear"));
        choices.add (TRANS ("Balanced"));
        choices.add (TRANS ("-2.5 dBFS Center"));
        choices.add (TRANS ("-3.0 dBFS Center"));
        choices.add (TRANS ("-4.5 dBFS Center"));
        choices.add (TRANS ("-6.0 dBFS Center"));
        choices.add (TRANS ("-3.0 dBFS Square"));
        choices.add (TRANS ("-4.5 dBFS Square"));

        return choices;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanRuleParameter)
};

//==============================================================================
PanProcessor::PanProcessor()
{
    panParam = new PanParameter();
    panRuleParam = new PanRuleParameter();

    addParameter (panParam);
    addParameter (panRuleParam);
}

//==============================================================================
void PanProcessor::setPan (float newPan)
{
    panParam->AudioParameterFloat::operator= (newPan);
}

void PanProcessor::setPanLaw (dsp::PannerRule newRule)
{
    panRuleParam->AudioParameterChoice::operator= (static_cast<int> (newRule));
}

float PanProcessor::getPan() const noexcept
{
    return panParam->get();
}

dsp::PannerRule PanProcessor::getPannerRule() const noexcept
{
    return static_cast<dsp::PannerRule> (panRuleParam->getIndex());
}

//==============================================================================
const String PanProcessor::getName() const
{
    return TRANS ("Stereophonic Panner");
}

Identifier PanProcessor::getIdentifier() const
{
    return "StereophonicPanner";
}

void PanProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, samplesPerBlock);

    const auto maxChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels());

    panner.prepare ({ sampleRate, (uint32) samplesPerBlock, (uint32) maxChans });
}

void PanProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    panner.setPan (getPan());
    panner.setRule (getPannerRule());

    dsp::AudioBlock<float> block (buffer);
    dsp::ProcessContextReplacing<float> context (block);
    panner.process (context);
}
