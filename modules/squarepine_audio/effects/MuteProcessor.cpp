MuteProcessor::MuteProcessor (bool startMuted) :
    InternalProcessor (false)
{
    auto layout = createDefaultParameterLayout();

    auto vp = std::make_unique<AudioParameterBool> (getIdentifier().toString(), getName(), false);
    muteParameter = vp.get();
    setMuted (startMuted);

    layout.add (std::move (vp));

    apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
}

//==============================================================================
void MuteProcessor::setMuted (bool shouldBeMuted)
{
    if (isMuted() == shouldBeMuted)
        return;

    if (shouldBeMuted)
        shouldFadeOut.store (true, std::memory_order_relaxed);
    else
        shouldFadeIn.store (true, std::memory_order_relaxed);

    muteParameter->operator= (shouldBeMuted);
}

bool MuteProcessor::isMuted() const noexcept
{
    return muteParameter->get();
}

//==============================================================================
void MuteProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiBuffer)
{
    process (buffer, midiBuffer);
}

void MuteProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiBuffer)
{
    process (buffer, midiBuffer);
}

template<typename FloatType>
void MuteProcessor::process (juce::AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages)
{
    auto appendAllNotesOff = [&]()
    {
        for (int i = 1; i <= 16; ++i)
            midiMessages.addEvent (MidiMessage::allNotesOff (i), 0);

        // Not all plugins/systems understand the All Notes Off message, so add note-offs.
        for (int i = 1; i <= 16; ++i)
            for (int f = 0; f <= 127; ++f)
                midiMessages.addEvent (MidiMessage::noteOff (i, f), 0);
    };

    constexpr auto zero = static_cast<FloatType> (0);
    constexpr auto one = static_cast<FloatType> (1);

    if (isMuted())
    {
        midiMessages.clear();

        if (shouldFadeOut.load (std::memory_order_relaxed))
        {
            buffer.applyGainRamp (0, buffer.getNumSamples(), one, zero);
            appendAllNotesOff();
        }
        else
        {
            buffer.clear();
        }
    }
    else
    {
        if (shouldFadeIn.load (std::memory_order_relaxed))
            buffer.applyGainRamp (0, buffer.getNumSamples(), zero, one);
    }

    shouldFadeIn.store (false, std::memory_order_relaxed);
    shouldFadeOut.store (false, std::memory_order_relaxed);
}
