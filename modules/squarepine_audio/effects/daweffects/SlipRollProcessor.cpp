namespace djdawprocessor
{

SlipRollProcessor::SlipRollProcessor (int idNum)
: idNumber (idNum)
{
    reset();
    
    NormalisableRange<float> wetDryRange = { 0.f, 1.f };
    auto wetdry = std::make_unique<NotifiableAudioParameterFloat> ("dryWet", "Dry/Wet", wetDryRange, 0.5f,
                                                                   true,// isAutomatable
                                                                   "Dry/Wet",
                                                                   AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String
                                                                   {
        int percentage = roundToInt (value * 100);
        String txt (percentage);
        return txt << "%";
    });
    
    auto fxon = std::make_unique<AudioParameterBool> ("fxonoff", "FX On", true, "FX On/Off ", [] (bool value, int) -> String
                                                      {
        if (value > 0)
            return TRANS ("On");
        return TRANS ("Off");
        ;
    });
    
    StringArray options { "1/16", "1/8", "1/4", "1/2", "1", "2", "4", "8", "16" };
    auto beat = std::make_unique<AudioParameterChoice> ("beat", "Beat Division", options, 3);
    
    NormalisableRange<float> timeRange = { 10.f, 4000.f };
    auto time = std::make_unique<NotifiableAudioParameterFloat> ("time", "Time", timeRange, 10.f,
                                                                 true,// isAutomatable
                                                                 "Time ",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String
                                                                 {
        String txt (roundToInt (value));
        return txt << "ms";
        ;
    });
    
    NormalisableRange<float> otherRange = { 0.f, 1.0f };
    auto other = std::make_unique<NotifiableAudioParameterFloat> ("x Pad", "X Pad Division", otherRange, 3,
                                                                  false,// isAutomatable
                                                                  "X Pad Division ",
                                                                  AudioProcessorParameter::genericParameter,
                                                                  [] (float value, int) -> String
                                                                  {
        int val = roundToInt (value);
        String txt;
        switch (val)
        {
            case 0:
                txt = "1/16";
                break;
            case 1:
                txt = "1/8";
                break;
            case 2:
                txt = "1/4";
                break;
            case 3:
                txt = "1/2";
                break;
            case 4:
                txt = "1";
                break;
            case 5:
                txt = "2";
                break;
            case 6:
                txt = "4";
                break;
            case 7:
                txt = "8";
                break;
            case 8:
                txt = "16";
                break;
            default:
                txt = "1";
                break;
        }
        
        return txt;
    });
    auto onoff = std::make_unique<AudioParameterBool> ("fx active", "FX Active", true, "FX Active ", [] (bool value, int) -> String
                                                       {
        if (value > 0)
            return TRANS ("Active");
        return TRANS ("Disabled");
        ;
    });
    
    wetDryParam = wetdry.get();
    wetDryParam->addListener (this);
    
    fxOnParam = fxon.get();
    fxOnParam->addListener (this);
    
    beatParam = beat.get();
    beatParam->addListener (this);
    
    timeParam = time.get();
    timeParam->addListener (this);
    
    xPadParam = other.get();
    xPadParam->addListener (this);
    
    onOffParam = onoff.get();
    onOffParam->addListener (this);
    
    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (fxon));
    layout.add (std::move (wetdry));
    layout.add (std::move (beat));
    layout.add (std::move (time));
    layout.add (std::move (other));
    setupBandParameters (layout);
    layout.add (std::move (onoff));
    
    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
    
    setPrimaryParameter (wetDryParam);
}

SlipRollProcessor::~SlipRollProcessor()
{
    wetDryParam->removeListener (this);
    fxOnParam->removeListener (this);
    beatParam->removeListener (this);
    timeParam->removeListener (this);
    xPadParam->removeListener (this);
    onOffParam->removeListener (this);
}

//============================================================================== Audio processing
void SlipRollProcessor::prepareToPlay (double Fs, int bufferSize)
{
    BandProcessor::prepareToPlay (Fs, bufferSize);
}
void SlipRollProcessor::processAudioBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String SlipRollProcessor::getName() const { return TRANS ("Slip Roll"); }
/** @internal */
Identifier SlipRollProcessor::getIdentifier() const { return "Slip Roll" + String (idNumber); }
/** @internal */
bool SlipRollProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void SlipRollProcessor::parameterValueChanged (int id, float value)
{
    //If the beat division is changed, the delay time should be set.
    //If the X Pad is used, the beat div and subsequently, time, should be updated.
    
    //Subtract the number of new parameters in this processor
    BandProcessor::parameterValueChanged (id, value);
}

}
