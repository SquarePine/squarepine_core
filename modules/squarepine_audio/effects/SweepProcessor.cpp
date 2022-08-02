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

    colourParam = colour.get();
    colourParam->addListener (this);

    fxFrequencyParam = fx.get();
    fxFrequencyParam->addListener (this);

    otherParam = other.get();
    otherParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (wetdry));
    layout.add (std::move (fx));
    layout.add (std::move (colour));
    layout.add (std::move (other));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

SweepProcessor::~SweepProcessor()
{
    wetDryParam->removeListener (this);
    fxFrequencyParam->removeListener (this);
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

const String SweepProcessor::getName() const { return TRANS ("SweepProcessor"); }
/** @internal */
Identifier SweepProcessor::getIdentifier() const { return "SweepProcessor" + String (idNumber); }
/** @internal */
bool SweepProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void SweepProcessor::parameterValueChanged (int, float)
{
}
