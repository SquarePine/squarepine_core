namespace djdawprocessor
{

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
    auto lowFrequencyToggle = std::make_unique<AudioParameterBool> ("lowonoff", "Low Frequency Processing", true, "Low Frequency Processing ", [] (bool value, int) -> String
                                                                    {
                                                                        if (value > 0)
                                                                            return TRANS ("On");
                                                                        return TRANS ("Off");
                                                                        ;
                                                                    });

    auto midFrequencyToggle = std::make_unique<AudioParameterBool> ("midonoff", "Mid Frequency Processing", true, "Mid Frequency Processing ", [] (bool value, int) -> String
                                                                    {
                                                                        if (value > 0)
                                                                            return TRANS ("On");
                                                                        return TRANS ("Off");
                                                                        ;
                                                                    });

    auto highFrequencyToggle = std::make_unique<AudioParameterBool> ("highonoff", "High Frequency Processing", true, "High Frequency Processing ", [] (bool value, int) -> String
                                                                     {
                                                                         if (value > 0)
                                                                             return TRANS ("On");
                                                                         return TRANS ("Off");
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

}
