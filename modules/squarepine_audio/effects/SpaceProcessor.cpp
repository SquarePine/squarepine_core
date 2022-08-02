SpaceProcessor::SpaceProcessor (int idNum): idNumber (idNum)
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
    auto fx = std::make_unique<NotifiableAudioParameterFloat> ("fx frequency", "FX Frequency", fxRange, 0.f,
                                                               true,// isAutomatable
                                                               "FX Frequency ",
                                                               AudioProcessorParameter::genericParameter,
                                                               [] (float value, int) -> String {
                                                                   int percentage = roundToInt (value * 100);
                                                                   String txt (percentage);
                                                                   return txt << "%";
                                                                   ;
                                                                   ;
                                                               });

    NormalisableRange<float> reverbRange = { -1.0, 1.0f };
    auto reverbColour = std::make_unique<NotifiableAudioParameterFloat> ("reverb colour", "Reverb Colour", reverbRange, 0.f,
                                                                         true,// isAutomatable
                                                                         "Reverb Colour ",
                                                                         AudioProcessorParameter::genericParameter,
                                                                         [] (float value, int) -> String {
                                                                             String txt (value);
                                                                             return txt;
                                                                             ;
                                                                         });

    NormalisableRange<float> feedbackRange = { 0.f, 1.0f };
    auto feedback = std::make_unique<NotifiableAudioParameterFloat> ("feedback", "Feedback", feedbackRange, 0.5f,
                                                                     true,// isAutomatable
                                                                     "Feedback ",
                                                                     AudioProcessorParameter::genericParameter,
                                                                     [] (float value, int) -> String {
                                                                         String txt (roundToInt (value));
                                                                         return txt << "%";
                                                                         ;
                                                                         ;
                                                                     });

    wetDryParam = wetdry.get();
    wetDryParam->addListener (this);

    reverbColourParam = reverbColour.get();
    reverbColourParam->addListener (this);

    fxFrequencyParam = fx.get();
    fxFrequencyParam->addListener (this);

    feedbackParam = feedback.get();
    feedbackParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (wetdry));
    layout.add (std::move (fx));
    layout.add (std::move (reverbColour));
    layout.add (std::move (feedback));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

SpaceProcessor::~SpaceProcessor()
{
    wetDryParam->removeListener (this);
    reverbColourParam->removeListener (this);
    fxFrequencyParam->removeListener (this);
    feedbackParam->removeListener (this);
}

//============================================================================== Audio processing
void SpaceProcessor::prepareToPlay (double, int)
{
}
void SpaceProcessor::processBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String SpaceProcessor::getName() const { return TRANS ("SpaceProcessor"); }
/** @internal */
Identifier SpaceProcessor::getIdentifier() const { return "SpaceProcessor" + String (idNumber); }
/** @internal */
bool SpaceProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void SpaceProcessor::parameterValueChanged (int, float)
{
}
