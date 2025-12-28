BitCrusherProcessor::BitCrusherProcessor() :
    InternalProcessor (false)
{
    presets =
    {
        { NEEDS_TRANS ("CleanPass"), 16.0f, 1.0f, 1.0f },
        { NEEDS_TRANS ("PixelPulse"), 8.0f, 12.0f, 2.5f },
        { NEEDS_TRANS ("PocketConsole"), 5.0f, 16.0f, 3.0f },
        { NEEDS_TRANS ("Lofi"), 10.0f, 4.0f, 6.0f },
        { NEEDS_TRANS ("WoodgrainFury"), 4.0f, 32.0f, 6.0f },
        { NEEDS_TRANS ("BrokenDAC"), 3.0f, 64.0f, 10.0f },
        { NEEDS_TRANS ("AliasBoi"), 4.0f, 72.0f, 10.0f }
    };

    auto layout = createDefaultParameterLayout();

    auto addFloatParam = [&] (StringRef id, StringRef name,
                              float start, float end, float defaultValue,
                              float interval, float skew)
    {
        auto newParam = std::make_unique<AudioParameterFloat> (id, name,
                                                               NormalisableRange<float> (start, end, interval, skew),
                                                               defaultValue);
        auto* np = newParam.get();
        layout.add (std::move (newParam));
        return np;
    };

    bitDepthParam           = addFloatParam ("depth", NEEDS_TRANS ("Depth"), 4.0f, 16.0f, 8.0f, 1.0f, 0.4f);
    downsampleFactorParam   = addFloatParam ("downsampleFactor", NEEDS_TRANS ("Downsample Factor"),  1.0f, 128.0f, 8.0f, 1.0f, 0.4f);
    driveParam              = addFloatParam ("drive", NEEDS_TRANS ("Drive"), 1.0f, 20.0f, 2.5f, 0.001f, 0.5f);

    resetAPVTSWithLayout (std::move (layout));

    setCurrentProgramDirectly (1);
}

//==============================================================================
int BitCrusherProcessor::getNumPrograms()       { return std::max (1, (int) presets.size()); }
int BitCrusherProcessor::getCurrentProgram()    { return programIndex; }

void BitCrusherProcessor::setCurrentProgramDirectly (int index)
{
    programIndex = index;
    const auto& p = presets[(PresetContainerSizeType) index];
    setBitDepth ((int) p.bitDepth);
    setDownsampleFactor ((int) p.downsample);
    setDrive ((float) p.drive);
}

void BitCrusherProcessor::setCurrentProgram (int index)
{
    // Don't check if programIndex is already set;
    // doing this allows "resetting" to the program.
    if (isPositiveAndBelow (index, (int) presets.size()))
        setCurrentProgramDirectly (index);
}

const String BitCrusherProcessor::getProgramName (int index) 
{
    if (isPositiveAndBelow (index, (int) presets.size()))
        return TRANS (presets[(PresetContainerSizeType) index].name);

    return {};
}

//==============================================================================
void BitCrusherProcessor::prepareToPlay (const double sampleRate, const int estimatedSamplesPerBlock)
{
    InternalProcessor::prepareToPlay (sampleRate, estimatedSamplesPerBlock);

    const auto tc = jmax (2, getTotalNumInputChannels(), getTotalNumOutputChannels());
    floatStates.resize (tc);
    doubleStates.resize (tc);
}

void BitCrusherProcessor::setBitDepth (int v)                   { bitDepthParam->operator= ((float) v); }
int BitCrusherProcessor::getBitDepth() const noexcept           { return (int) bitDepthParam->get(); }
void BitCrusherProcessor::setDownsampleFactor (int v)           { downsampleFactorParam->operator= ((float) v); }
int BitCrusherProcessor::getDownsampleFactor() const noexcept   { return (int) downsampleFactorParam->get(); }
void BitCrusherProcessor::setDrive (float v)                    { driveParam->operator= (v); }
float BitCrusherProcessor::getDrive() const noexcept            { return driveParam->get(); }

//==============================================================================
void BitCrusherProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)  { process<float> (buffer, floatStates); }
void BitCrusherProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&) { process<double> (buffer, doubleStates); }

template<typename FloatType>
FloatType BitCrusherProcessor::crush (FloatType sample, ChannelState<FloatType>& state,
                                      FloatType levels, FloatType makeup,
                                      FloatType drive, int dsFactor)
{
    // Pre-gain + saturation
    sample = juce::dsp::FastMathApproximations::tanh (sample * drive);

    // Sample-rate reduction (ZOH)
    if (--state.holdCounter <= 0)
    {
        state.heldSample = sample;
        state.holdCounter = dsFactor; // precomputed int >= 1
    }

    // Bit depth reduction
    // levels = 1 << bits
    sample = std::round (state.heldSample * levels) / levels;

    // Makeup gain
    sample *= makeup; // 1 / sqrt(drive)

    return sample;
}

template<typename FloatType>
void BitCrusherProcessor::process (juce::AudioBuffer<FloatType>& buffer, Array<ChannelState<FloatType>>& states)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();
    if (isBypassed()
        || numChannels <= 0
        || numSamples <= 0
        || buffer.hasBeenCleared())
        return; // Nothing to do here.

    const auto localBitDepth = getBitDepth();
    const auto localDrive = (FloatType) getDrive();
    const auto localDownsampleFactor = getDownsampleFactor();
    const auto levels = FloatType (1 << localBitDepth);
    const auto makeup = FloatType (1) / std::sqrt (localDrive);

    auto chans = buffer.getArrayOfWritePointers();

    for (int i = 0; i < numChannels; ++i)
    {
        if (auto chan = chans[i])
        {
            auto& state = states.getReference (i);
            for (int f = 0; f < numSamples; ++f)
                chan[f] = crush (chan[f], state, levels, makeup, localDrive, localDownsampleFactor);
        }
    }
}

