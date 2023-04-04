namespace djdawprocessor
{

EffectiveTempoProcessor::EffectiveTempoProcessor()
    : InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    float minValue = -100.f;
    float maxValue = 100.f;
    float defaultValue = 0.f;
    String name = "EffectiveTempo";
    auto vp = std::make_unique<NotifiableAudioParameterFloat> (getIdentifier().toString(),
                                                               name,
                                                               minValue,
                                                               maxValue,
                                                               defaultValue,
                                                               true);

    effectiveTempoParameter = vp.get();
    effectiveTempoParameter->addListener (this);

    layout.add (std::move (vp));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}

//==============================================================================
void EffectiveTempoProcessor::prepareToPlay (double newSampleRate, int bufferSize)
{
    const ScopedLock lock (getCallbackLock());

    setRateAndBufferSizeDetails (newSampleRate, bufferSize);

    const auto numChannels = jmax (2, getTotalNumInputChannels(), getTotalNumOutputChannels());

    floatChannelDetails.prepare (numChannels);
    doubleChannelDetails.prepare (numChannels);
}

//==============================================================================
void EffectiveTempoProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatChannelDetails);
}

void EffectiveTempoProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleChannelDetails);
}

void EffectiveTempoProcessor::parameterValueChanged (int, float)
{
}

void EffectiveTempoProcessor::parameterGestureChanged (int, bool)
{
}

}
