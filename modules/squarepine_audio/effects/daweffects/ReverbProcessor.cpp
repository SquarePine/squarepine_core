namespace djdawprocessor
{
ReverbProcessor::ReverbProcessor (int idNum)
    : idNumber (idNum)
{
    reset();

    NormalisableRange<float> wetDryRange = { 0.f, 1.f };
    auto wetdry = std::make_unique<NotifiableAudioParameterFloat> ("dryWet", "Dry/Wet", wetDryRange, 0.5f,
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

    NormalisableRange<float> reverbAmountRange = { 0.f, 1 };
    auto reverbAmount = std::make_unique<NotifiableAudioParameterFloat> ("amount", "Reverb Amount ", reverbAmountRange, 0.5,
                                                                         true,// isAutomatable
                                                                         "Reverb Filter Amount ",
                                                                         AudioProcessorParameter::genericParameter,
                                                                         [] (float value, int) -> String
                                                                         {
                                                                             int percentage = roundToInt (value * 100);
                                                                             String txt (percentage);
                                                                             return txt << "%";
                                                                         });

    NormalisableRange<float> timeRange = { 0, 1.0f };
    auto time = std::make_unique<NotifiableAudioParameterFloat> ("time", "Time", timeRange, 0.5f,
                                                                 true,// isAutomatable
                                                                 "Time ",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String
                                                                 {
                                                                     int percentage = roundToInt (value * 100);
                                                                     String txt (percentage);
                                                                     return txt << "%";
                                                                 });

    NormalisableRange<float> otherRange = { 0.f, 1.0f };
    auto other = std::make_unique<NotifiableAudioParameterFloat> ("x Pad", "Cutoff", otherRange, 3,
                                                                  true,// isAutomatable
                                                                  "X Pad Division ",
                                                                  AudioProcessorParameter::genericParameter,
                                                                  [] (float value, int) -> String
                                                                  {
                                                                      String txt (roundToInt (value));
                                                                      return txt;
                                                                  });

    wetDryParam = wetdry.get();
    wetDryParam->addListener (this);

    reverbAmountParam = reverbAmount.get();
    reverbAmountParam->addListener (this);

    timeParam = time.get();
    timeParam->addListener (this);

    xPadParam = other.get();
    xPadParam->addListener (this);

    fxOnParam = fxon.get();
    fxOnParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (fxon));
    layout.add (std::move (wetdry));
    layout.add (std::move (reverbAmount));
    layout.add (std::move (time));
    layout.add (std::move (other));
    setupBandParameters (layout);
    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    setPrimaryParameter (wetDryParam);
}

ReverbProcessor::~ReverbProcessor()
{
    wetDryParam->removeListener (this);
    fxOnParam->removeListener (this);
    reverbAmountParam->removeListener (this);
    timeParam->removeListener (this);
    xPadParam->removeListener (this);
}

//============================================================================== Audio processing
void ReverbProcessor::prepareToPlay (double Fs, int bufferSize)
{
    setRateAndBufferSizeDetails (Fs, bufferSize);
    BandProcessor::prepareToPlay (Fs, bufferSize);
    reverb.reset();
    reverb.setSampleRate (Fs);
}
void ReverbProcessor::processAudioBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (isBypassed()
        || buffer.hasBeenCleared()
        || numChannels <= 0
        || numSamples <= 0)
        return;

    updateReverbParams();

    auto** chans = buffer.getArrayOfWritePointers();

    const ScopedLock sl (getCallbackLock());

    switch (numChannels)
    {
        case 1:
            reverb.processMono (chans[0], numSamples);
            break;

        case 2:
            reverb.processStereo (chans[0], chans[1], numSamples);
            break;

        default:
            break;
    }
}

const String ReverbProcessor::getName() const { return TRANS ("Reverb"); }
/** @internal */
Identifier ReverbProcessor::getIdentifier() const { return "Reverb" + String (idNumber); }
/** @internal */
bool ReverbProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void ReverbProcessor::parameterValueChanged (int id, float value)
{
    //If the beat division is changed, the delay time should be set.
    //If the X Pad is used, the beat div and subsequently, time, should be updated.
    if (id == 1)
    {
        setBypass (value > 0);
    }
    //Subtract the number of new parameters in this processor
    BandProcessor::parameterValueChanged (id, value);
}
void ReverbProcessor::releaseResources()
{
    const ScopedLock sl (getCallbackLock());
    reverb.reset();
}

void ReverbProcessor::updateReverbParams()
{
    Reverb::Parameters localParams;

    localParams.roomSize = timeParam->get();
    localParams.damping = 1 - reverbAmountParam->get();
    localParams.wetLevel = wetDryParam->get();
    localParams.dryLevel = 1 - wetDryParam->get();
    localParams.width = 1;
    localParams.freezeMode = 0;

    {
        const ScopedLock sl (getCallbackLock());
        reverb.setParameters (localParams);
    }
}
}
