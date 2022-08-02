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

    NormalisableRange<float> fxRange = { 0.f, 1.0f };
    auto fx = std::make_unique<NotifiableAudioParameterFloat> ("fx frequency", "FX Frequency", fxRange, 0.5f,
                                                               true,// isAutomatable
                                                               "FX Frequency ",
                                                               AudioProcessorParameter::genericParameter,
                                                               [] (float value, int) -> String {
                                                                   String txt (roundToInt (value));
                                                                   return txt << "%";
                                                                   ;
                                                                   ;
                                                               });

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

    NormalisableRange<float> otherRange = { 0.f, 1.0f };
    auto other = std::make_unique<NotifiableAudioParameterFloat> ("other", "other", otherRange, 0.5f,
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

    colourParam = colour.get();
    colourParam->addListener (this);

    fxFrequencyParam = fx.get();
    fxFrequencyParam->addListener (this);

    emphasisParam = other.get();
    emphasisParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (wetdry));
    layout.add (std::move (fx));
    layout.add (std::move (colour));
    layout.add (std::move (other));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

CrushProcessor::~CrushProcessor()
{
    wetDryParam->removeListener (this);
    fxFrequencyParam->removeListener (this);
    colourParam->removeListener (this);
    emphasisParam->removeListener (this);
}

//============================================================================== Audio processing
void CrushProcessor::prepareToPlay (double, int)
{
}
void CrushProcessor::processBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String CrushProcessor::getName() const { return TRANS ("CrushProcessor"); }
/** @internal */
Identifier CrushProcessor::getIdentifier() const { return "CrushProcessor" + String (idNumber); }
/** @internal */
bool CrushProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void CrushProcessor::parameterValueChanged (int, float)
{
}
