//==============================================================================
class PanProcessor::PanParameter final : public AudioParameterFloat
{
public:
    PanParameter() :
        AudioParameterFloat ("pan", TRANS ("Pan"), fullLeft, fullRight, centre)
    {
    }

    String getLabel() const override { return getGeneralPanPosition(); }

    String getText (float v, int maximumStringLength) const override
    {
        // we will need at least 6 characters to show 3 digits + "% L" or "% R"
        if (maximumStringLength <= 6)
            return getGeneralPanPosition();
        
        const auto range = getNormalisableRange().getRange();
        const auto value = jmap (v, 0.0f, 1.0f, range.getStart(), range.getEnd());
        String l = "Centre";
        
        if (approximatelyEqual (value, centre))
            return l;

        if (value < centre)
            l = String (abs(static_cast<int>(value * 100.0f))).substring (0, maximumStringLength - 3) + "% L";
        else if (value > centre)
            l = String (abs(static_cast<int>(value * 100.0f))).substring (0, maximumStringLength - 3) + "% R";

        return l;
    }

private:
    String getGeneralPanPosition() const
    {
        const auto v = get();

        if (approximatelyEqual (v, centre))        return "C";
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
        AudioParameterChoice ("panRule", TRANS ("Pan Rule"), getChoices(),
                              static_cast<int> (PanProcessor::defaultPannerRule))
    {
    }

private:
    static StringArray getChoices()
    {
        StringArray choices;
        choices.add (TRANS ("Linear"));
        choices.add (TRANS ("Balanced"));
        choices.add (TRANS ("-3.0 dBFS"));
        choices.add (TRANS ("-4.5 dBFS"));
        choices.add (TRANS ("-6.0 dBFS"));
        choices.add (TRANS ("-3.0 dBFS Square"));
        choices.add (TRANS ("-4.5 dBFS Square"));

        return choices;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanRuleParameter)
};

//==============================================================================
PanProcessor::PanProcessor() :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto pp = std::make_unique<PanParameter>();
    panParam = pp.get();
    layout.add (std::move (pp));

    auto prp = std::make_unique<PanRuleParameter>();
    panRuleParam = prp.get();
    layout.add (std::move (prp));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}

PanProcessor::~PanProcessor()
{
}

//==============================================================================
void PanProcessor::setPan (float newPan)
{
    panParam->AudioParameterFloat::operator= (newPan);
}

void PanProcessor::setPannerRule (dsp::PannerRule newRule)
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
void PanProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, samplesPerBlock);

    const auto maxChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels());

    const dsp::ProcessSpec spec =
    {
        sampleRate,
        (uint32) samplesPerBlock,
        (uint32) maxChans
    };

    floatPanner.prepare (spec);
    doublePanner.prepare (spec);
}

void PanProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiBuffer)
{
    process (floatPanner, buffer, midiBuffer);
}

void PanProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiBuffer)
{
    process (doublePanner, buffer, midiBuffer);
}

template<typename FloatType>
void PanProcessor::process (dsp::Panner<FloatType>& panner,
                            juce::AudioBuffer<FloatType>& buffer,
                            MidiBuffer&)
{
    panner.setPan (getPan());
    panner.setRule (getPannerRule());

    dsp::AudioBlock<FloatType> block (buffer);
    dsp::ProcessContextReplacing<FloatType> context (block);
    context.isBypassed = isBypassed();
    panner.process (context);
}
