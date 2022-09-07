SweepProcessor::SweepProcessor (int idNum): idNumber (idNum)
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

    auto fxon = std::make_unique<AudioParameterBool> ("fxonoff", "FX On",true,
                                                                 "FX On/Off ",
                                                                 [] (bool value, int) -> String {
                                                                     if (value > 0)
                                                                         return TRANS("On");
                                                                     return TRANS("Off");
                                                                     ;
                                                                 });
    
    /*Turning the control to the left produces a gate effect, and turning it to the right produces a band pass filter effect.
    Turn counterclockwise: A gate effect makes the sound tighter, with a reduced sense of volume.
    Turn to right: The band pass filter bandwidth decreases steadily.
     */
    NormalisableRange<float> colourRange = { -1.0, 1.0f };
    auto colour = std::make_unique<NotifiableAudioParameterFloat> ("colour", "Colour", colourRange, 0.f,
                                                                   true,// isAutomatable
                                                                   "Colour ",
                                                                   AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String {
                                                                       String txt (roundToInt (value));
                                                                       return txt;
                                                                       ;
                                                                   });
    /*
    Turning the [COLOR] control to the left adjusts the gate effect.
    Turn to the right to tighten the sound.
    Turning the [COLOR] to the right adjusts the center frequency.
    Turn to the right to increase the center frequency.

    */

    NormalisableRange<float> otherRange = { 0.f, 1.0f };
    auto other = std::make_unique<NotifiableAudioParameterFloat> ("tighten", "Tighten", otherRange, 0.5f,
                                                                  true,// isAutomatable
                                                                  "Tighten ",
                                                                  AudioProcessorParameter::genericParameter,
                                                                  [] (float value, int) -> String {
                                                                      int percentage = roundToInt (value * 100);
                                                                      String txt (percentage);
                                                                      return txt << "%";
                                                                  });

    wetDryParam = wetdry.get();
    wetDryParam->addListener (this);

    fxOnParam = fxon.get();
    fxOnParam->addListener(this);
    
    
    colourParam = colour.get();
    colourParam->addListener (this);

    otherParam = other.get();
    otherParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (fxon));
    layout.add (std::move (wetdry));
    layout.add (std::move (colour));
    layout.add (std::move (other));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

SweepProcessor::~SweepProcessor()
{
    wetDryParam->removeListener (this);
    fxOnParam->removeListener(this);
    colourParam->removeListener (this);
    otherParam->removeListener (this);
}

//============================================================================== Audio processing
void SweepProcessor::prepareToPlay (double, int)
{
}
void SweepProcessor::processBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String SweepProcessor::getName() const { return TRANS ("Sweep"); }
/** @internal */
Identifier SweepProcessor::getIdentifier() const { return "Sweep" + String (idNumber); }
/** @internal */
bool SweepProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void SweepProcessor::parameterValueChanged (int, float)
{
}
