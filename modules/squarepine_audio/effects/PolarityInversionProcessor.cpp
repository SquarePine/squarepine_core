PolarityInversionProcessor::PolarityInversionProcessor (bool startActive) :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto vp = std::make_unique<AudioParameterBool> (getIdentifier().toString(), NEEDS_TRANS ("Invert"), true);
    invertParameter = vp.get();
    layout.add (std::move (vp));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setActive (startActive);
}

//==============================================================================
void PolarityInversionProcessor::setActive (bool shouldBeActive)
{
    invertParameter->operator= (shouldBeActive);
}

bool PolarityInversionProcessor::isActive() const
{
    return invertParameter->get();
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

template<typename FloatType>
void PolarityInversionProcessor::process (juce::AudioBuffer<FloatType>& buffer, MidiBuffer&)
{
    if (! isBypassed() && isActive())
        invertPolarity (buffer);
}
