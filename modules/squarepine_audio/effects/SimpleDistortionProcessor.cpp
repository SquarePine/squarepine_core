//==============================================================================
class SimpleDistortionProcessor::AmountParameter final : public AudioParameterFloat
{
public:
    AmountParameter() noexcept :
        AudioParameterFloat ("AmountId", TRANS ("Amount"), 0.0f, 1.0f, 0.6f)
    {
    }

    int getNumSteps() const override { return 100; }
    String getLabel() const override { return "%"; }

    String getText (float v, int maximumStringLength) const override
    {
        return String (static_cast<int> (v * 100.0f)).substring (0, maximumStringLength);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmountParameter)
};

//==============================================================================
SimpleDistortionProcessor::SimpleDistortionProcessor() :
    amountParam (new AmountParameter())
{
    AudioProcessor::addParameter (amountParam);
}

//==============================================================================
void SimpleDistortionProcessor::prepareToPlay (double newSampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, samplesPerBlock);

    const dsp::ProcessSpec spec =
    {
        newSampleRate,
        (uint32) samplesPerBlock,
        (uint32) jmax (getTotalNumInputChannels(), getTotalNumOutputChannels())
    };

    floatPackage.prepare (spec);
    doublePackage.prepare (spec);
}

template<typename FloatType>
void SimpleDistortionProcessor::process (juce::AudioBuffer<FloatType>& buffer, BufferPackage<FloatType>& package)
{
    if (isBypassed())
        return;
 
    {
        const dsp::AudioBlock<const FloatType> dry (buffer);
        package.mixer.pushDrySamples (dry);
    }

    package.wet = buffer;
    package.mixer.setWetMixProportion (static_cast<FloatType> (amountParam->get()));

    using DistFuncs = DistortionFunctions<FloatType>;
    DistFuncs::perform (package.wet, static_cast<FloatType> (1), DistFuncs::simple);
    dsp::AudioBlock<FloatType> wet (package.wet);
    package.mixer.mixWetSamples (wet);
    wet.copyTo (buffer);
}

void SimpleDistortionProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)    { process (buffer, floatPackage); }
void SimpleDistortionProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)   { process (buffer, doublePackage); }
