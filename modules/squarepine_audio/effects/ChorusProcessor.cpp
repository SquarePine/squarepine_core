//==============================================================================
ChorusProcessor::ChorusProcessor() :
    rate (new AudioParameterFloat ("rate", NEEDS_TRANS ("Rate"), 1.0f, 99.0f, 50.0f)),
    depth (new AudioParameterFloat ("depth", NEEDS_TRANS ("Depth"), 0.0f, 1.0f, 1.0f)),
    centreDelay (new AudioParameterFloat ("centreDelay", NEEDS_TRANS ("Centre Delay"), 1.0f, 99.0f, 50.0f)),
    feedback (new AudioParameterFloat ("feedback", NEEDS_TRANS ("Feedback"), -1.0f, 1.0f, 0.25f)),
    mix (new AudioParameterFloat ("mix", NEEDS_TRANS ("Mix"), 0.0f, 1.0f, 1.0f))
{
    AudioProcessor::addParameter (rate);
    AudioProcessor::addParameter (depth);
    AudioProcessor::addParameter (centreDelay);
    AudioProcessor::addParameter (feedback);
    AudioProcessor::addParameter (mix);

    setRate (2.5f);
    setDepth (0.5f);
    setCentreDelay (1.0f);
    setFeedback (0.25f);
    setMix (0.75f);
}

//==============================================================================
void ChorusProcessor::setRate (float newRateHz)
{
    if (rate->get() != newRateHz)
        rate->setValueNotifyingHost (newRateHz);

    const ScopedLock sl (getCallbackLock());
    floatChorus.setRate (rate->get());
    doubleChorus.setRate ((double) rate->get());
}

float ChorusProcessor::getRate() const noexcept
{
    return rate->get();
}

//==============================================================================
void ChorusProcessor::setDepth (float newDepth)
{
    if (depth->get() != newDepth)
        depth->setValueNotifyingHost (newDepth);

    const ScopedLock sl (getCallbackLock());
    floatChorus.setDepth (depth->get());
    doubleChorus.setDepth ((double) depth->get());
}

float ChorusProcessor::getDepth() const noexcept
{
    return depth->get();
}

//==============================================================================
void ChorusProcessor::setCentreDelay (float newDelayMs)
{
    if (centreDelay->get() != newDelayMs)
        centreDelay->setValueNotifyingHost (newDelayMs);

    const ScopedLock sl (getCallbackLock());
    floatChorus.setCentreDelay (centreDelay->get());
    doubleChorus.setCentreDelay ((double) centreDelay->get());
}

float ChorusProcessor::getCentreDelay() const noexcept
{
    return centreDelay->get();
}

//==============================================================================
void ChorusProcessor::setFeedback (float newFeedback)
{
    if (feedback->get() != newFeedback)
        feedback->setValueNotifyingHost (newFeedback);

    const ScopedLock sl (getCallbackLock());
    floatChorus.setFeedback (feedback->get());
    doubleChorus.setFeedback ((double) feedback->get());
}

float ChorusProcessor::getFeedback() const noexcept
{
    return feedback->get();
}

//==============================================================================
void ChorusProcessor::setMix (float newMix)
{
    if (mix->get() != newMix)
        mix->setValueNotifyingHost (newMix);

    const ScopedLock sl (getCallbackLock());
    floatChorus.setMix (mix->get());
    doubleChorus.setMix ((double) mix->get());
}

float ChorusProcessor::getMix() const noexcept
{
    return mix->get();
}

//==============================================================================
void ChorusProcessor::prepareToPlay (double sampleRate, int bufferSize)
{
    const ScopedLock sl (getCallbackLock());

    setRateAndBufferSizeDetails (sampleRate, bufferSize);

    const auto numChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels());

    const dsp::ProcessSpec spec =
    {
        sampleRate,
        (uint32) bufferSize,
        (uint32) numChans
    };

    floatChorus.prepare (spec);
    doubleChorus.prepare (spec);
}

void ChorusProcessor::releaseResources()
{
    const ScopedLock sl (getCallbackLock());

    floatChorus.reset();
    doubleChorus.reset();
}

//==============================================================================
void ChorusProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    process (buffer, floatChorus);
}

void ChorusProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)
{
    process (buffer, doubleChorus);
}

template<typename FloatType>
void ChorusProcessor::process (juce::AudioBuffer<FloatType>& buffer, dsp::Chorus<FloatType>& chorus)
{
    const ScopedLock sl (getCallbackLock());

    dsp::AudioBlock<FloatType> block (buffer);
    dsp::ProcessContextReplacing<FloatType> context (block);
    context.isBypassed = isBypassed();
    chorus.process (context);
}
