//==============================================================================
class LevelsProcessor::MeteringModeParameter final : public AudioParameterChoice
{
public:
    MeteringModeParameter() :
        AudioParameterChoice ("meteringModeId", NEEDS_TRANS ("Metering Mode"),
                              getChoices(), static_cast<int> (MeteringMode::peak))
    {
    }

private:
    static StringArray getChoices()
    {
        StringArray choices;
        choices.add (NEEDS_TRANS ("Peak"));
        choices.add (NEEDS_TRANS ("RMS"));
        choices.add (NEEDS_TRANS ("Mid/Side"));
        return choices;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeteringModeParameter)
};

//==============================================================================
LevelsProcessor::LevelsProcessor() :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto mmp = std::make_unique<MeteringModeParameter>();
    meteringModeParam = mmp.get();
    layout.add (std::move (mmp));

    resetAPVTSWithLayout (std::move (layout));
}

//==============================================================================
void LevelsProcessor::setMeteringMode (MeteringMode newMode)
{
    meteringModeParam->AudioParameterChoice::operator= (static_cast<int> (newMode));
}

MeteringMode LevelsProcessor::getMeteringMode() const
{
    return static_cast<MeteringMode> (meteringModeParam->getIndex());
}

void LevelsProcessor::getChannelLevels (Array<float>& destData)
{
    destData = floatChannelDetails.channels;
}

void LevelsProcessor::getChannelLevels (Array<double>& destData)
{
    destData = doubleChannelDetails.channels;
}

//==============================================================================
void LevelsProcessor::prepareToPlay (double newSampleRate, int newBufferSize)
{
    setRateAndBufferSizeDetails (newSampleRate, newBufferSize);

    const auto numChannels = std::max (getTotalNumInputChannels(), getTotalNumOutputChannels());

    floatChannelDetails.prepare (numChannels);
    doubleChannelDetails.prepare (numChannels);
}

void LevelsProcessor::processBlock (juce::AudioBuffer<float>& b, MidiBuffer&)   { process (b); }
void LevelsProcessor::processBlock (juce::AudioBuffer<double>& b, MidiBuffer&)  { process (b); }

template<typename FloatType>
void LevelsProcessor::process (juce::AudioBuffer<FloatType>& buffer)
{
    const auto mode = getMeteringMode();
    const auto b = isBypassed();

    floatChannelDetails.process (buffer, mode, b);
    doubleChannelDetails.process (buffer, mode, b);
}
