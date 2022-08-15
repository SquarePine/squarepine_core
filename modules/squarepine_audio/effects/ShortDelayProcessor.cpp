ShortDelayProcessor::ShortDelayProcessor (int idNum): idNumber (idNum)
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
    NormalisableRange<float> timeRange = { 1.f, 400.0f };
    auto time = std::make_unique<NotifiableAudioParameterFloat> ("delayTime", "Delay Time", timeRange, 200.f,
                                                                 true,// isAutomatable
                                                                 "Delay Time",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String {
                                                                     String txt (roundToInt (value));
                                                                     return txt << "ms";
                                                                     ;
                                                                 });
    NormalisableRange<float> colourRange = { -1.f, 1.0f };
    auto colour = std::make_unique<NotifiableAudioParameterFloat> ("colour", "Colour/Tone", colourRange, 0.f,
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

    colourParam = colour.get();
    colourParam->addListener (this);

    feedbackParam = feedback.get();
    feedbackParam->addListener (this);

    timeParam = time.get();
    timeParam->addListener(this);
    
    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (wetdry));
    layout.add (std::move (colour));
    layout.add (std::move (feedback));
    layout.add (std::move (time));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

ShortDelayProcessor::~ShortDelayProcessor()
{
    wetDryParam->removeListener (this);
    colourParam->removeListener (this);
    feedbackParam->removeListener (this);
    timeParam->removeListener(this);
}

//============================================================================== Audio processing
void ShortDelayProcessor::prepareToPlay (double, int)
{
}
void ShortDelayProcessor::processBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String ShortDelayProcessor::getName() const { return TRANS ("Short Delay"); }
/** @internal */
Identifier ShortDelayProcessor::getIdentifier() const { return "Short Delay" + String (idNumber); }
/** @internal */
bool ShortDelayProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void ShortDelayProcessor::parameterValueChanged (int, float)
{
}
