namespace djdawprocessor
{

LongDelayProcessor::LongDelayProcessor (int idNum)
    : idNumber (idNum)
{
    reset();

    NormalisableRange<float> wetDryRange = { 0.f, 1.f };
    auto wetdry = std::make_unique<NotifiableAudioParameterFloat> ("dryWetDelay", "Dry/Wet", wetDryRange, 0.5f,
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

    NormalisableRange<float> timeRange = { 400.f, 4000.0f };
    auto time = std::make_unique<NotifiableAudioParameterFloat> ("delayTime", "Delay Time", timeRange, 200.f,
                                                                 true,// isAutomatable
                                                                 "Delay Time",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String
                                                                 {
                                                                     String txt (roundToInt (value));
                                                                     return txt << "ms";
                                                                     ;
                                                                 });
    NormalisableRange<float> colourRange = { -1.f, 1.0f };
    auto colour = std::make_unique<NotifiableAudioParameterFloat> ("colour", "Colour/Tone", colourRange, 0.f,
                                                                   true,// isAutomatable
                                                                   "Colour ",
                                                                   AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String
                                                                   {
                                                                       String txt (value);
                                                                       return txt;
                                                                       ;
                                                                   });

    NormalisableRange<float> feedbackRange = { 0.f, 1.0f };
    auto feedback = std::make_unique<NotifiableAudioParameterFloat> ("feedback", "Feedback", feedbackRange, 0.5f,
                                                                     true,// isAutomatable
                                                                     "Feedback ",
                                                                     AudioProcessorParameter::genericParameter,
                                                                     [] (float value, int) -> String
                                                                     {
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

    feedbackParam = feedback.get();
    feedbackParam->addListener (this);

    timeParam = time.get();
    timeParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (fxon));
    layout.add (std::move (wetdry));
    layout.add (std::move (colour));
    layout.add (std::move (feedback));
    layout.add (std::move (time));
    appendExtraParams (layout);
    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

LongDelayProcessor::~LongDelayProcessor()
{
    wetDryParam->removeListener (this);
    fxOnParam->removeListener (this);
    colourParam->removeListener (this);
    feedbackParam->removeListener (this);
    timeParam->removeListener (this);
}

//============================================================================== Audio processing
void LongDelayProcessor::prepareToPlay (double, int)
{
}
void LongDelayProcessor::processBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String LongDelayProcessor::getName() const { return TRANS ("Long Delay"); }
/** @internal */
Identifier LongDelayProcessor::getIdentifier() const { return "Long Delay" + String (idNumber); }
/** @internal */
bool LongDelayProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void LongDelayProcessor::parameterValueChanged (int, float)
{
}

}
