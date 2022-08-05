NoiseProcessor::NoiseProcessor (int idNum): idNumber (idNum)
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
    /*
     Turn counterclockwise: The cut-off frequency of the filter through which the white noise passes gradually descends.
     Turn clockwise: The cut-off frequency of the filter through which the white noise passes gradually rises.
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

    NormalisableRange<float> noiseRange = { -96.f, 6.0f };
    auto noise = std::make_unique<NotifiableAudioParameterFloat> ("other", "Noise Volume", noiseRange, -6.f,
                                                                  true,// isAutomatable
                                                                  "Noise Volume ",
                                                                  AudioProcessorParameter::genericParameter,
                                                                  [] (float value, int) -> String {
                                                                      if (approximatelyEqual (value, 0.0f))
                                                                          return "0 dB";

                                                                      if (approximatelyEqual (value, -96.0f))
                                                                          return "-Inf dB";

                                                                      return Decibels::toString (value);
                                                                  });

    wetDryParam = wetdry.get();
    wetDryParam->addListener (this);

    colourParam = colour.get();
    colourParam->addListener (this);


    volumeParam = noise.get();
    volumeParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (wetdry));
    layout.add (std::move (colour));
    layout.add (std::move (noise));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

NoiseProcessor::~NoiseProcessor()
{
    wetDryParam->removeListener (this);
    colourParam->removeListener (this);
    volumeParam->removeListener (this);
}

//============================================================================== Audio processing
void NoiseProcessor::prepareToPlay (double, int)
{
}
void NoiseProcessor::processBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String NoiseProcessor::getName() const { return TRANS ("Noise"); }
/** @internal */
Identifier NoiseProcessor::getIdentifier() const { return "Noise" + String (idNumber); }
/** @internal */
bool NoiseProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void NoiseProcessor::parameterValueChanged (int, float)
{
}
