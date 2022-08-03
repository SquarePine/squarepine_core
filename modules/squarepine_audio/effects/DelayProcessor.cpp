FractionalDelay::FractionalDelay()
{
}
// Destructor
FractionalDelay::~FractionalDelay()
{
}

float FractionalDelay::processSample (float x, int channel)
{
    smoothDelay[channel] = 0.999f * smoothDelay[channel] + 0.001f * delay;

    if (smoothDelay[channel] < 1.f)
    {
        return x;
    }
    else
    {
        // Delay Buffer
        // "delay" can be fraction
        int d1 = (int)floor (smoothDelay[channel]);
        int d2 = d1 + 1;
        float g2 = smoothDelay[channel] - (float) d1;
        float g1 = 1.0f - g2;

        int indexD1 = index[channel] - d1;
        if (indexD1 < 0)
        {
            indexD1 += MAX_BUFFER_SIZE;
        }

        int indexD2 = index[channel] - d2;
        if (indexD2 < 0)
        {
            indexD2 += MAX_BUFFER_SIZE;
        }

        float y = g1 * delayBuffer[indexD1][channel] + g2 * delayBuffer[indexD2][channel];

        delayBuffer[index[channel]][channel] = x;

        if (index[channel] < MAX_BUFFER_SIZE - 1)
        {
            index[channel]++;
        }
        else
        {
            index[channel] = 0;
        }

        return y;
    }
}

void FractionalDelay::setFs (float Fs)
{
    this->Fs = Fs;
}

void FractionalDelay::setDelaySamples (float delay)
{
    if (delay >= 1.f)
    {
        this->delay = delay;
    }
    else
    {
        this->delay = 0.f;
    }
}

DelayProcessor::DelayProcessor (int idNum): idNumber (idNum)
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

    NormalisableRange<float> timeRange = { 1.f, 4000.0f };
    auto time = std::make_unique<NotifiableAudioParameterFloat> ("delayTime", "Delay Time", timeRange, 200.f,
                                                                 true,// isAutomatable
                                                                 "Delay Time",
                                                                 AudioProcessorParameter::genericParameter,
                                                                 [] (float value, int) -> String {
                                                                     String txt (roundToInt (value));
                                                                     return txt << "ms";
                                                                     ;
                                                                 });

    delayUnit.setDelaySamples (200 * 48);
    wetDry.setTargetValue (0.5);
    delayTime.setTargetValue (200 * 48);

    wetDryParam = wetdry.get();
    wetDryParam->addListener (this);

    delayTimeParam = time.get();
    delayTimeParam->addListener (this);

    auto layout = createDefaultParameterLayout (false);
    layout.add (std::move (wetdry));
    layout.add (std::move (time));
    setupBandParameters(layout);
    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));

    
    setPrimaryParameter (wetDryParam);
}

DelayProcessor::~DelayProcessor()
{
    wetDryParam->removeListener (this);
    delayTimeParam->removeListener (this);
}

//============================================================================== Audio processing
void DelayProcessor::prepareToPlay (double Fs, int bufferSize)
{
    const ScopedLock lock (getCallbackLock());
    BandProcessor::prepareToPlay(Fs, bufferSize);
    
    delayUnit.setFs ((float) Fs);
    wetDry.reset (Fs, 0.001f);
    delayTime.reset (Fs, 0.001f);
    setRateAndBufferSizeDetails (Fs, bufferSize);
}
void DelayProcessor::processAudioBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    //TODO
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    const ScopedLock lock (getCallbackLock());

    float dry, wet, x, y;
    for (int s = 0; s < numSamples; ++s)
    {
        delayUnit.setDelaySamples (delayTime.getNextValue());
        wet = wetDry.getNextValue();
        dry = 1.f - wet;
        for (int c = 0; c < numChannels; ++c)
        {
            x = buffer.getWritePointer (c)[s];
            y = (delayUnit.processSample (x, c) * wet) + (x * dry);
            buffer.getWritePointer (c)[s] = y;
        }
    }
}
//============================================================================== House keeping
const String DelayProcessor::getName() const { return TRANS ("Delay"); }
/** @internal */
Identifier DelayProcessor::getIdentifier() const { return "delay" + String (idNumber); }
/** @internal */
bool DelayProcessor::supportsDoublePrecisionProcessing() const { return false; }
//============================================================================== Parameter callbacks
void DelayProcessor::parameterValueChanged (int paramNum, float value)
{
    const ScopedLock sl (getCallbackLock());
    
    switch (paramNum)
    {
        case 1:
            //wet dry
            wetDry.setTargetValue (jlimit (0.f, 1.f, value));
            break;
        case 2:
        {
            //delay time
            auto range = delayTimeParam->getNormalisableRange().getRange();
            auto valMS = (float) jlimit ((int) range.getStart(), (int) range.getEnd(), roundToInt (value));
            auto time = (getSampleRate() / 1000) * valMS;
            delayTime.setTargetValue ((float) time);
            break;
        }
        default:
            break;
    }
    //Subtract the number of new parameters in this processor
    BandProcessor::parameterValueChanged(paramNum - 2, value);
}
