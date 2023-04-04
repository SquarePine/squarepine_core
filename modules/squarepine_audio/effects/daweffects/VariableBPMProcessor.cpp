namespace djdawprocessor
{


VariableBPMProcessor::VariableBPMProcessor() :
InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();
    
    float minValue = -100.f;
    float maxValue = 100.f;
    float defaultValue = 0.f;
    String name = "VariableBPM";
    auto vp = std::make_unique<NotifiableAudioParameterFloat> (getIdentifier().toString(),
                                                               name,
                                                               minValue,
                                                               maxValue,
                                                               defaultValue,
                                                               true);
    
    variableBPMParameter = vp.get();
    variableBPMParameter->addListener (this);
    
    layout.add (std::move (vp));
    
    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}


//==============================================================================
void VariableBPMProcessor::prepareToPlay (double newSampleRate, int bufferSize)
{
    const ScopedLock lock (getCallbackLock());
    
    setRateAndBufferSizeDetails (newSampleRate, bufferSize);
    
    const auto numChannels = jmax (2, getTotalNumInputChannels(), getTotalNumOutputChannels());
    
    floatChannelDetails.prepare (numChannels);
    doubleChannelDetails.prepare (numChannels);
}

//==============================================================================
void VariableBPMProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatChannelDetails);
}

void VariableBPMProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleChannelDetails);
}

void VariableBPMProcessor::parameterValueChanged (int, float)
{
}

void VariableBPMProcessor::parameterGestureChanged (int, bool)
{
}

}
