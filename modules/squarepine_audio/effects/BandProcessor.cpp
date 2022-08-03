
BandProcessor::BandProcessor()
{
}
BandProcessor::~BandProcessor()
{
    if (lowFrequencyToggleParam != nullptr)
        lowFrequencyToggleParam->removeListener (this);
    if (midFrequencyToggleParam != nullptr)
        midFrequencyToggleParam->removeListener (this);
    if (highFrequencyToggleParam != nullptr)
        lowFrequencyToggleParam->removeListener (this);
}

void BandProcessor::setupBandParameters (AudioProcessorValueTreeState::ParameterLayout& layout)
{
    NormalisableRange<float> onoffRange = { 0.f, 1.0f };
    auto lowFrequencyToggle = std::make_unique<NotifiableAudioParameterFloat> ("lowonoff", "Low Frequency Processing", onoffRange, 0,
                                                                               true,// isAutomatable
                                                                               "Low Frequency Processing",
                                                                               AudioProcessorParameter::genericParameter,
                                                                               [] (float value, int) -> String {
                                                                                   if (value > 0)
                                                                                       return "On";
                                                                                   return "Off";
                                                                                   ;
                                                                               });
    NormalisableRange<float> onoffRange2 = { 0.f, 1.0f };
    auto midFrequencyToggle = std::make_unique<NotifiableAudioParameterFloat> ("midonoff", "Mid Frequency Processing", onoffRange2, 0,
                                                                               true,// isAutomatable
                                                                               "Mid Frequency Processing",
                                                                               AudioProcessorParameter::genericParameter,
                                                                               [] (float value, int) -> String {
                                                                                   if (value > 0)
                                                                                       return "On";
                                                                                   return "Off";
                                                                                   ;
                                                                               });
    NormalisableRange<float> onoffRange3 = { 0.f, 1.0f };

    auto highFrequencyToggle = std::make_unique<NotifiableAudioParameterFloat> ("highonoff", "High Frequency Processing", onoffRange3, 0,
                                                                                true,// isAutomatable
                                                                                "High Frequency Processing",
                                                                                AudioProcessorParameter::genericParameter,
                                                                                [] (float value, int) -> String {
                                                                                    if (value > 0)
                                                                                        return "On";
                                                                                    return "Off";
                                                                                    ;
                                                                                });

    lowFrequencyToggleParam = lowFrequencyToggle.get();
    lowFrequencyToggleParam->addListener (this);

    midFrequencyToggleParam = midFrequencyToggle.get();
    midFrequencyToggleParam->addListener (this);

    highFrequencyToggleParam = highFrequencyToggle.get();
    highFrequencyToggleParam->addListener (this);

    layout.add (std::move (lowFrequencyToggle));
    layout.add (std::move (midFrequencyToggle));
    layout.add (std::move (highFrequencyToggle));
}

void BandProcessor::parameterValueChanged (int paramIndex, float)
{
    const ScopedLock sl (getCallbackLock());
    switch (paramIndex)
    {
        case (1):
            //Low frequency toggle changed
            break;
        case (2):
            //Mid frequency toggle changed
            break;
        case (3):
            //High frequency toggle changed
            break;
    }
}

void BandProcessor::prepareToPlay (double Fs, int bufferSize)
{
    setRateAndBufferSizeDetails (Fs, bufferSize);
}
void BandProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    //This should split the signal into bands if required
    //Then the processaudioblock should be called to perform the processing

    //TODO
    processAudioBlock (buffer, midi);
}
