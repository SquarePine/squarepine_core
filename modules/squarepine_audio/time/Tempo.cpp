//==============================================================================
namespace details
{
    double snapValue (double t) noexcept
    {
        if (std::isnan (t) || std::isinf (t))
            return Tempo::defaultTempo;

        return std::clamp (t, Tempo::minimumTempo, Tempo::maximumTempo);
    }
}

//==============================================================================
Tempo::Tempo (double tempo) noexcept :
    value (details::snapValue (tempo))
{
}

//==============================================================================
bool Tempo::operator== (const Tempo& other) const noexcept { return value == other.value; }
bool Tempo::operator!= (const Tempo& other) const noexcept { return ! operator== (other); }
bool Tempo::operator< (const Tempo& other) const noexcept  { return value < other.value; }
bool Tempo::operator<= (const Tempo& other) const noexcept { return value <= other.value; }
bool Tempo::operator> (const Tempo& other) const noexcept  { return value > other.value; }
bool Tempo::operator>= (const Tempo& other) const noexcept { return value >= other.value; }

//==============================================================================
Tempo Tempo::fromReader (const AudioFormatReader& reader)
{
    const auto& metadata = reader.metadataValues;

    if (reader.getFormatName().equalsIgnoreCase ("wav"))
    {
        if (metadata.containsKey (WavAudioFormat::acidTempo))
            return Tempo ((double) metadata[WavAudioFormat::acidTempo].getIntValue());
    }
   #if SQUAREPINE_USE_REX_AUDIO_FORMAT
    else if (reader.getFormatName().equalsIgnoreCase ("rex"))
    {
        if (metadata.containsKey (REXAudioFormat::rexTempo))
            return Tempo ((double) metadata[REXAudioFormat::rexTempo].getIntValue());
    }
   #endif
   #if JUCE_MAC || JUCE_IOS
    if (metadata.containsKey (CoreAudioFormat::tempo))
        return Tempo ((double) metadata[CoreAudioFormat::tempo].getIntValue());
   #endif

    return {};
}

Tempo Tempo::fromMIDIFile (const MidiFile& midiFile)
{
    for (int i = 0; i < midiFile.getNumTracks(); ++i)
    {
        auto track = *midiFile.getTrack (i);
        track.sort();

        for (auto m : track)
            if (m->message.isTempoMetaEvent())
                return Tempo (m->message.getTempoSecondsPerQuarterNote());
    }

    jassertfalse;
    return {};
}
