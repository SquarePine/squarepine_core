PitchProcessor::PitchProcessor (int idNum): idNumber (idNum)
{
    reset();

    NormalisableRange<float> wetDryRange = { 0.f, 1.f };
    auto wetdry = std::make_unique<NotifiableAudioParameterFloat> ("dryWet", "Dry/Wet", wetDryRange, 0.5f,
                                                                   true,// isAutomatable
                                                                   "Dry/Wet",
                                                                   AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String {
                                                                       int percentage = roundToInt (value * 100);
                                                                       String txt (percentage);
                                                                       return txt << "%";
                                                                   });
    
    auto fxon = std::make_unique<AudioParameterBool> ("fxonoff", "FX On",true,
                                                                 "FX On/Off ",
                                                                 [] (bool value, int) -> String {
                                                                     if (value > 0)
                                                                         return TRANS("On");
                                                                     return TRANS("Off");
                                                                     ;
                                                                 });
    

    NormalisableRange<float> beatRange = { -50, 100 };
    auto beat = std::make_unique<NotifiableAudioParameterFloat> ("beat", "beat", beatRange, 0,
                                                                 false,// isAutomatable
                                                                 "Beat Division ",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String {
                                                                     String txt (roundToInt (value));
                                                                     return txt;
                                                                 });

    NormalisableRange<float> timeRange = { -50.f, 100.0f };
    auto time = std::make_unique<NotifiableAudioParameterFloat> ("pitch", "Pitch", timeRange, 0.f,
                                                                 true,// isAutomatable
                                                                 "Pitch ",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String {
                                                                     String txt (roundToInt (value));
                                                                     return txt << "%";
                                                                     ;
                                                                 });

    NormalisableRange<float> otherRange = { 0.f, 1.0f };
    auto other = std::make_unique<NotifiableAudioParameterFloat> ("x Pad", "X Pad Division", beatRange, 3,
                                                                  false,// isAutomatable
                                                                  "X Pad Division ",
                                                                  AudioProcessorParameter::genericParameter,
                                                                  [] (float value, int) -> String {
                                                                      String txt (roundToInt (value));
                                                                      return txt;
                                                                  });

    wetDryParam = wetdry.get();
    wetDryParam->addListener (this);
    
    fxOnParam = fxon.get();
    fxOnParam->addListener(this);

    beatParam = beat.get();
    beatParam->addListener (this);

    timeParam = time.get();
    timeParam->addListener (this);

    xPadParam = other.get();
    xPadParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (fxon));
    layout.add (std::move (wetdry));
    layout.add (std::move (beat));
    layout.add (std::move (time));
    layout.add (std::move (other));
    setupBandParameters(layout);
    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

PitchProcessor::~PitchProcessor()
{
    wetDryParam->removeListener (this);
    fxOnParam->removeListener(this);
    beatParam->removeListener (this);
    timeParam->removeListener (this);
    xPadParam->removeListener (this);
}

//============================================================================== Audio processing
void PitchProcessor::prepareToPlay (double Fs, int bufferSize)
{
    BandProcessor::prepareToPlay(Fs, bufferSize);
}
void PitchProcessor::processAudioBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String PitchProcessor::getName() const { return TRANS ("Pitch"); }
/** @internal */
Identifier PitchProcessor::getIdentifier() const { return "Pitch" + String (idNumber); }
/** @internal */
bool PitchProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void PitchProcessor::parameterValueChanged (int id, float value)
{
    //If the beat division is changed, the delay time should be set.
    //If the X Pad is used, the beat div and subsequently, time, should be updated.
    
    //Subtract the number of new parameters in this processor
    BandProcessor::parameterValueChanged (id, value);
}
