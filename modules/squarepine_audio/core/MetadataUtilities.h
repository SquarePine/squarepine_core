//==============================================================================
/** */
inline [[nodiscard]] String getTitle (const MidiFile& midiFile)
{
    for (int i = 0; i < midiFile.getNumTracks(); ++i)
    {
        const auto* track = midiFile.getTrack (i);

        for (const auto* meh : *track)
        {
            const auto& msg = meh->message;
            if (msg.isTextMetaEvent())
                return msg.getTextFromTextMetaEvent();
        }
    }

    return {};
}

/** */
inline [[nodiscard]] String getTitle (const AudioFormatReader& reader)
{
    String title;

    auto hasKeyAndSetTitle = [&] (StringRef key)
    {
        if (title.isEmpty() && reader.metadataValues.containsKey (key))
            title = reader.metadataValues[key].trim();

        return title.isNotEmpty();
    };

    // Common metadata keys:
    if (reader.getFormatName().equalsIgnoreCase ("wav"))
    {
        if (hasKeyAndSetTitle (WavAudioFormat::bwavDescription))
            return title;

        if (hasKeyAndSetTitle (WavAudioFormat::riffInfoTitle))
            return title;
    }
   #if JUCE_USE_OGGVORBIS
    else if (reader.getFormatName().equalsIgnoreCase ("ogg"))
    {
        if (hasKeyAndSetTitle (OggVorbisAudioFormat::id3title))
            return title;
    }
   #endif

    // Generic keys, very crappy cases:
    if (hasKeyAndSetTitle ("Name"))
        return title;
    if (hasKeyAndSetTitle ("Title"))
        return title;

    // Failure case, best guess:
    if (auto* is = reader.input)
        if (auto* fis = dynamic_cast<FileInputStream*> (is))
            return fis->getFile().getFileNameWithoutExtension().trim();

    return {};
}

//==============================================================================
/** @todo */
inline [[nodiscard]] int64 getNumBeats (const AudioFormatReader& reader)
{
    if (reader.getFormatName().containsIgnoreCase ("AIFF"))
        if (reader.metadataValues.containsKey (AiffAudioFormat::appleBeats))
            return reader.metadataValues[AiffAudioFormat::appleBeats].getLargeIntValue();

    /*
    const auto tempo = getFirstKnownTempo (reader);
    if (tempo <= 0.0)
        return 0;

    const auto ts = getFirstTimeSignature (reader);
    if (ts.numerator <= 0 || ts.denominator <= 0)
        return 0;

    return calculateBeats (timeSamplesToSeconds (reader),
                           getFirstKnownTempo (reader),
                           ts);
    */
    return 0;
}
