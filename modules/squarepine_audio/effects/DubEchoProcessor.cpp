DubEchoProcessor::DubEchoProcessor (int idNum): idNumber (idNum)
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

    NormalisableRange<float> colourRange = { -1.f, 1.0f };
    auto colour = std::make_unique<NotifiableAudioParameterFloat> ("colour", "Colour", colourRange, 0.f,
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

    echoColourParam = colour.get();
    echoColourParam->addListener (this);

    feedbackParam = feedback.get();
    feedbackParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (wetdry));
    layout.add (std::move (colour));
    layout.add (std::move (feedback));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

DubEchoProcessor::~DubEchoProcessor()
{
    wetDryParam->removeListener (this);
    echoColourParam->removeListener (this);
    feedbackParam->removeListener (this);
}

//============================================================================== Audio processing
void DubEchoProcessor::prepareToPlay (double, int)
{
}
void DubEchoProcessor::processBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String DubEchoProcessor::getName() const { return TRANS ("Dub Echo"); }
/** @internal */
Identifier DubEchoProcessor::getIdentifier() const { return "Dub Echo" + String (idNumber); }
/** @internal */
bool DubEchoProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void DubEchoProcessor::parameterValueChanged (int, float)
{
}
