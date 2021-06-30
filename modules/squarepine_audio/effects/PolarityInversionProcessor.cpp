PolarityInversionProcessor::PolarityInversionProcessor (bool startActive) :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto vp = std::make_unique<AudioParameterBool> (getIdentifier().toString(), getName(), false);
    invertParameter = vp.get();
    setActive (startActive);

    layout.add (std::move (vp));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}

//==============================================================================
void PolarityInversionProcessor::setActive (bool shouldBeActive)
{
    invertParameter->operator= (shouldBeActive);
}

bool PolarityInversionProcessor::isActive() const noexcept
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
