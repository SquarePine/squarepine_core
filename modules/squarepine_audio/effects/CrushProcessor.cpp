CrushProcessor::CrushProcessor (int idNum): idNumber (idNum)
{
    reset();

    NormalisableRange<float> wetDryRange = { 0.f, 1.f };
    auto wetdry = std::make_unique<NotifiableAudioParameterFloat> ("dryWetDelay", "Dry/Wet", wetDryRange, 0.5f,
                                                                   true,// isAutomatable
                                                                   "Dry/Wet",
                                                                   AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String {
                                                                       int percentage = roundToInt (value * 100);
                                                                       String txt (percentage);
                                                                       return txt << "%";
                                                                   });

    auto fxon = std::make_unique<NotifiableAudioParameterBool> ("fxonoff", "FX On", true, "FX On/Off ", true, [] (bool value, int) -> String {
        if (value > 0)
            return TRANS ("On");
        return TRANS ("Off");
        ;
    });

    /*
     Turn counterclockwise: Increases the sound’s distortion.
     Turn clockwise: The sound is crushed before passing through the high pass filter.
     */
    NormalisableRange<float> colourRange = { -1.0, 1.0f };
    auto colour = std::make_unique<NotifiableAudioParameterFloat> ("colour", "Colour", colourRange, 0.f,
                                                                   true,// isAutomatable
                                                                   "Colour ",
                                                                   AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String {
                                                                       String txt (std::round (100.f*value) / 100.f);
                                                                       return txt;
                                                                       ;
                                                                   });

    NormalisableRange<float> otherRange = { 0.f, 1.0f };
    auto other = std::make_unique<NotifiableAudioParameterFloat> ("other", "Noise Volume", otherRange, 0.5f,
                                                                  true,// isAutomatable
                                                                  "Other ",
                                                                  AudioProcessorParameter::genericParameter,
                                                                  [] (float value, int) -> String {
                                                                      int percentage = roundToInt (value * 100);
                                                                      String txt (percentage);
                                                                      return txt << "%";
                                                                  });

    wetDryParam = wetdry.get();
    wetDryParam->addListener (this);

    fxOnParam = fxon.get();
    fxOnParam->addListener (this);

    colourParam = colour.get();
    colourParam->addListener (this);

    emphasisParam = other.get();
    emphasisParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (fxon));
    layout.add (std::move (wetdry));
    layout.add (std::move (colour));
    layout.add (std::move (other));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

CrushProcessor::~CrushProcessor()
{
    wetDryParam->removeListener (this);
    fxOnParam->removeListener (this);
    colourParam->removeListener (this);
    emphasisParam->removeListener (this);
}

//============================================================================== Audio processing
void CrushProcessor::prepareToPlay (double sampleRate, int bufferSize)
{
    bitCrusher.prepareToPlay (sampleRate, bufferSize);
    highPassFilter.setFilterType (DigitalFilter::FilterType::HPF);
    highPassFilter.setFs (sampleRate);
    
    dryBuffer = AudioBuffer<float> (2, bufferSize);
}
void CrushProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    float wet = 0.5f;
    bool isOn = false;
    {
        const ScopedLock sl (getCallbackLock());
        wet = wetDryParam->get();
        isOn = fxOnParam->get();
    }
        
    if (!isOn)
        return;
    
    for (int c = 0 ; c < buffer.getNumChannels(); ++c)
    {
        dryBuffer.copyFrom (c,0, buffer,c,0,buffer.getNumSamples());
    }
    bitCrusher.processBlock (buffer, midi);
    highPassFilter.processBuffer (buffer, midi);
    
    buffer.applyGain (wet);
    dryBuffer.applyGain (1.f-wet);
    
    for (int c = 0 ; c < buffer.getNumChannels(); ++c)
    {
        buffer.addFrom (c,0, dryBuffer,c,0,buffer.getNumSamples());
    }
}

const String CrushProcessor::getName() const { return TRANS ("Crush"); }
/** @internal */
Identifier CrushProcessor::getIdentifier() const { return "Crush" + String (idNumber); }
/** @internal */
bool CrushProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void CrushProcessor::parameterValueChanged (int paramNum, float value)
{
    const ScopedLock sl (getCallbackLock());
    if (paramNum == 2) {}// wet/dry
    else if (paramNum == 3) // "color"
    {
        if (value <= 0.f)
        {
            highPassFilter.setFreq (20.0);
            float normValue = (value * -1.f);
            // 3 bits -> normValue = 0
            // 8 bits -> normValue = 1
            bitCrusher.setBitDepth (5.f * std::sqrt(1.f - normValue) + 3.f);
        }
        else
        {
            float normValue = value;
            // freqHz = 20 -> 5000
            float freqHz = 2.f * std::powf(10.f,3.f * (normValue * 0.8f) + 1.f);
            highPassFilter.setFreq (freqHz);
            // 8 bits -> value = 0.5, normValue = 0
            // 3 bits -> value = 1, normValue = 0
            bitCrusher.setBitDepth (5.f * std::sqrt(1.f - normValue) + 3.f);
        }
    }
}

