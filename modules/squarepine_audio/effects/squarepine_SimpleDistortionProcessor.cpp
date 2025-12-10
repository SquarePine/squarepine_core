//==============================================================================
namespace
{
    String toStringFromDistortionAmountValue (float value, int maximumStringLength)
    {
        auto s = String (static_cast<int> (value * 100.0f))
                    .substring (0, maximumStringLength - 1);
        s << "%";
        return s;
    }
}

//==============================================================================
SimpleDistortionProcessor::SimpleDistortionProcessor() :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto ap = std::make_unique<AudioParameterFloat> (ParameterID ("amount", 1), TRANS ("Amount"),
                                                     NormalisableRange<float> (0.0f, 100.0f, 1.0f), 75.0f,
                                                     AudioParameterFloatAttributes()
                                                        .withLabel (TRANS ("Amount"))
                                                        .withStringFromValueFunction (toStringFromDistortionAmountValue));
    amountParameter = ap.get();
    layout.add (std::move (ap));
    resetAPVTSWithLayout (std::move (layout));
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
    const auto d = static_cast<FloatType> (amountParameter->get());
    DistFuncs::perform (buffer, DistFuncs::sigmoid, d, static_cast<FloatType> (100));
}

void SimpleDistortionProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)    { process (buffer); }
void SimpleDistortionProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)   { process (buffer); }
