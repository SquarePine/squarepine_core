//==============================================================================
class PanProcessor::PanParameter final : public AudioParameterFloat
{
public:
    PanParameter() :
        AudioParameterFloat ("pan", TRANS ("Pan"),
                             { fullLeft, fullRight, 0.01f },
                             centre)
    {
    }

    String getLabel() const override { return getGeneralPanPosition(); }

    String getText (float v, int maximumStringLength) const override
    {
        if (maximumStringLength <= 6)
            return getGeneralPanPosition().substring (0, maximumStringLength);

        v = convertFrom0to1 (v);

        auto createSnappedValueString = [] (float valueToSnap, StringRef labelName)
        {
            const auto vr = roundToIntAccurate (std::abs (valueToSnap) * 100.0f);
            return String (vr) + "% " + labelName;
        };

        auto l = TRANS ("Centre");

        if (v < centre)         l = createSnappedValueString (v, "L");
        else if (v > centre)    l = createSnappedValueString (v, "R");

        return l.substring (0, maximumStringLength);
    }

private:
    static String getGeneralPanPosition (float v)
    {
        if (v < centre)         return "L";
        else if (v > centre)    return "R";
        else                    return "C";
    }

    String getGeneralPanPosition() const
    {
        return getGeneralPanPosition (convertFrom0to1 (get()));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanParameter)
};

//==============================================================================
class PanProcessor::PanRuleParameter final : public AudioParameterChoice
{
public:
    PanRuleParameter() :
        AudioParameterChoice ("panRule", TRANS ("Pan Rule"), createChoices(),
                              static_cast<int> (PanProcessor::defaultPannerRule))
    {
    }

    String getText (float v, int) const override
    {
        const auto index = (int) convertFrom0to1 (v);
        return TRANS (choices[index]);
    }

private:
    static StringArray createChoices()
    {
        StringArray c;
        c.add (NEEDS_TRANS ("Linear"));
        c.add (NEEDS_TRANS ("Balanced"));
        c.add (NEEDS_TRANS ("-3.0 dBFS"));
        c.add (NEEDS_TRANS ("-4.5 dBFS"));
        c.add (NEEDS_TRANS ("-6.0 dBFS"));
        c.add (NEEDS_TRANS ("-3.0 dBFS Square"));
        c.add (NEEDS_TRANS ("-4.5 dBFS Square"));
        return c;
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

    resetAPVTSWithLayout (std::move (layout));
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
void PanProcessor::prepareToPlay (double newSampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, samplesPerBlock);

    const dsp::ProcessSpec spec =
    {
        newSampleRate,
        (uint32) samplesPerBlock,
        (uint32) jmax (getTotalNumInputChannels(), getTotalNumOutputChannels())
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
    if (isBypassed())
        return;

    panner.setPan (getPan());
    panner.setRule (getPannerRule());

    dsp::AudioBlock<FloatType> block (buffer);
    dsp::ProcessContextReplacing<FloatType> context (block);
    context.isBypassed = isBypassed();
    panner.process (context);
}
