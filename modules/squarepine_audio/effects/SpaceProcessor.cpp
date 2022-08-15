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

    NormalisableRange<float> fxOnRange = { 0.f, 1.0f };

    auto fxon = std::make_unique<NotifiableAudioParameterFloat> ("fxonoff", "FX On", fxOnRange, 1,
                                                                  true,// isAutomatable
                                                                  "FX On/Off ",
                                                                  AudioProcessorParameter::genericParameter,
                                                                  [] (float value, int) -> String {
                                                                      if (value > 0)
                                                                          return "On";
                                                                      return "Off";
                                                                      ;
                                                                  });
    
    
    NormalisableRange<float> timeRange = { 1.f, 4000.0f };
    auto time = std::make_unique<NotifiableAudioParameterFloat> ("delayTime", "Reverb/Space Time", timeRange, 200.f,
                                                                 true,// isAutomatable
                                                                 "Delay Time",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String {
                                                                     String txt (roundToInt (value));
                                                                     return txt << "ms";
                                                                     ;
                                                                 });
    
    NormalisableRange<float> reverbRange = { -1.0, 1.0f };
    auto reverbColour = std::make_unique<NotifiableAudioParameterFloat> ("reverb colour", "Colour/Tone", reverbRange, 0.f,
                                                                         true,// isAutomatable
                                                                         "Colour ",
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
                                                                         int percentage = roundToInt (value * 100);
                                                                         String txt (percentage);
                                                                         return txt << "%";
                                                                     });

    wetDryParam = wetdry.get();
    wetDryParam->addListener (this);

    fxOnParam = fxon.get();
    fxOnParam->addListener(this);
    
    reverbColourParam = reverbColour.get();
    reverbColourParam->addListener (this);

    feedbackParam = feedback.get();
    feedbackParam->addListener (this);
    
    timeParam = time.get();
    timeParam->addListener(this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (fxon));
    layout.add (std::move (wetdry));
    layout.add (std::move (reverbColour));
    layout.add (std::move (feedback));
    layout.add (std::move (time));


    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

SpaceProcessor::~SpaceProcessor()
{
    wetDryParam->removeListener (this);
    fxOnParam->removeListener(this);
    reverbColourParam->removeListener (this);
    feedbackParam->removeListener (this);
    timeParam->removeListener(this);
}

//============================================================================== Audio processing
void SpaceProcessor::prepareToPlay (double, int)
{
}
void SpaceProcessor::processBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String SpaceProcessor::getName() const { return TRANS ("Space"); }
/** @internal */
Identifier SpaceProcessor::getIdentifier() const { return "Space" + String (idNumber); }
/** @internal */
bool SpaceProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void SpaceProcessor::parameterValueChanged (int, float)
{
}
