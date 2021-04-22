//==============================================================================
class MuteProcessor::MuteParameter : public AudioParameterBool
{
public:
    MuteParameter() :
        AudioParameterBool ("muteId", TRANS ("Mute"), false)
    {
    }

    String getText (float v, int maximumStringLength) const override
    {
        return (v >= 0.5f ? TRANS ("Muted") : TRANS ("Normal"))
               .substring (0, maximumStringLength);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MuteParameter)
};

//==============================================================================
MuteProcessor::MuteProcessor() :
    muteParameter (new MuteParameter())
{
    addParameter (muteParameter);
}

//==============================================================================
void MuteProcessor::setMuted (const bool shouldBeMuted)
{
    if (isMuted() == shouldBeMuted || muteParameter == nullptr)
        return;

    if (shouldBeMuted)
        shouldFadeOut.store (true, std::memory_order_relaxed);
    else
        shouldFadeIn.store (true, std::memory_order_relaxed);

    muteParameter->juce::AudioParameterBool::operator= (shouldBeMuted);
}

bool MuteProcessor::isMuted() const
{
    if (muteParameter != nullptr)
        return muteParameter->get();

    return false;
}

//==============================================================================
void MuteProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
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

    if (muteParameter->get())
    {
        midiMessages.clear();

        if (shouldFadeOut.load (std::memory_order_relaxed))
        {
            buffer.applyGainRamp (0, buffer.getNumSamples(), 1.0f, 0.0f);
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
            buffer.applyGainRamp (0, buffer.getNumSamples(), 0.0f, 1.0f);
    }

    shouldFadeIn.store (false, std::memory_order_relaxed);
    shouldFadeOut.store (false, std::memory_order_relaxed);
}
