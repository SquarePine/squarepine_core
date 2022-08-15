SpiralProcessor::SpiralProcessor (int idNum): idNumber (idNum)
{
    reset();

    
    //The wet dry in this instance should also alter the amount of feedback being used.
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

    NormalisableRange<float> beatRange = { 0.f, 8.0 };
    auto beat = std::make_unique<NotifiableAudioParameterFloat> ("beat", "Beat Division", beatRange, 3,
                                                                 false,// isAutomatable
                                                                 "Beat Division ",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String {
                                                                     int val = roundToInt (value);
                                                                     String txt;
                                                                     switch (val)
                                                                     {
                                                                         case 0:
                                                                             txt = "1/16";
                                                                             break;
                                                                         case 1:
                                                                             txt = "1/8";
                                                                             break;
                                                                         case 2:
                                                                             txt = "1/4";
                                                                             break;
                                                                         case 3:
                                                                             txt = "1/2";
                                                                             break;
                                                                         case 4:
                                                                             txt = "1";
                                                                             break;
                                                                         case 5:
                                                                             txt = "2";
                                                                             break;
                                                                         case 6:
                                                                             txt = "4";
                                                                             break;
                                                                         case 7:
                                                                             txt = "8";
                                                                             break;
                                                                         case 8:
                                                                             txt = "16";
                                                                             break;
                                                                         default:
                                                                             txt = "1";
                                                                             break;
                                                                     }

                                                                     return txt;
                                                                 });

    NormalisableRange<float> timeRange = { 10.f, 4000.f };
    auto time = std::make_unique<NotifiableAudioParameterFloat> ("time", "Time", timeRange, 10.f,
                                                                 true,// isAutomatable
                                                                 "Time ",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String {
                                                                     String txt (roundToInt (value));
                                                                     return txt<< "ms";
                                                                     ;
                                                                 });

    NormalisableRange<float> otherRange = { 0.f, 1.0f };
    auto other = std::make_unique<NotifiableAudioParameterFloat> ("x Pad", "X Pad Division", beatRange, 3,
                                                                  false,// isAutomatable
                                                                  "X Pad Division ",
                                                                  AudioProcessorParameter::genericParameter,
                                                                  [] (float value, int) -> String {
                                                                      int val = roundToInt (value);
                                                                      String txt;
                                                                      switch (val)
                                                                      {
                                                                          case 0:
                                                                              txt = "1/16";
                                                                              break;
                                                                          case 1:
                                                                              txt = "1/8";
                                                                              break;
                                                                          case 2:
                                                                              txt = "1/4";
                                                                              break;
                                                                          case 3:
                                                                              txt = "1/2";
                                                                              break;
                                                                          case 4:
                                                                              txt = "1";
                                                                              break;
                                                                          case 5:
                                                                              txt = "2";
                                                                              break;
                                                                          case 6:
                                                                              txt = "4";
                                                                              break;
                                                                          case 7:
                                                                              txt = "8";
                                                                              break;
                                                                          case 8:
                                                                              txt = "16";
                                                                              break;
                                                                          default:
                                                                              txt = "1";
                                                                              break;
                                                                      }

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

SpiralProcessor::~SpiralProcessor()
{
    wetDryParam->removeListener (this);
    fxOnParam->removeListener(this);
    beatParam->removeListener (this);
    timeParam->removeListener (this);
    xPadParam->removeListener (this);
}

//============================================================================== Audio processing
void SpiralProcessor::prepareToPlay (double Fs, int bufferSize)
{
    BandProcessor::prepareToPlay(Fs, bufferSize);
}
void SpiralProcessor::processAudioBlock (juce::AudioBuffer<float>&, MidiBuffer&)
{
}

const String SpiralProcessor::getName() const { return TRANS ("Spiral"); }
/** @internal */
Identifier SpiralProcessor::getIdentifier() const { return "Spiral" + String (idNumber); }
/** @internal */
bool SpiralProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void SpiralProcessor::parameterValueChanged (int id, float value)
{
    //If the beat division is changed, the delay time should be set.
    //If the X Pad is used, the beat div and subsequently, time, should be updated.
    
    //Subtract the number of new parameters in this processor
    BandProcessor::parameterValueChanged (id, value);
}
