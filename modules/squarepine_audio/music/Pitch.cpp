Pitch::Pitch (double frequencyHz) noexcept :
    frequency (std::max (0.0, frequencyHz))
{
}

Pitch Pitch::fromNoteName (const String& noteName)
{
    const auto pitchClassName = noteName.toLowerCase().retainCharacters (getValidPitchClassLetters());
    const auto pitchClass = getPitchClass (pitchClassName);

    if (pitchClass > 0)
    {
        const auto octave = noteName.retainCharacters ("0123456789").getIntValue();
        return fromMIDINote ((octave * 12) + pitchClass);
    }

    return {};
}

String Pitch::getMIDINoteName() const
{
    const auto midiNote = getMIDINote();
    const auto pitchClass = midiNote % 12;
    const auto octave = midiNote / 12 - 1;

    String s;
    s.preallocateBytes (4);
    s << getNoteName (pitchClass, true) << octave;
    return s.trim();
}

String Pitch::getNoteName (int pitchClass, bool asSharps)
{
    static const char* const sharpNoteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    static const char* const flatNoteNames[]  = { "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B" };

    if (isPositiveAndBelow (pitchClass, 12))
        return asSharps ? sharpNoteNames[pitchClass] : flatNoteNames[pitchClass];

    return {};
}

int Pitch::getPitchClass (const String& pitchClassName)
{
    int pitchClass = -1;
    const auto numChars = pitchClassName.length();

    if (numChars > 0)
    {
        switch (pitchClassName.toLowerCase()[0])
        {
            case 'c': pitchClass = 0; break;
            case 'd': pitchClass = 2; break;
            case 'e': pitchClass = 4; break;
            case 'f': pitchClass = 5; break;
            case 'g': pitchClass = 7; break;
            case 'a': pitchClass = 9; break;
            case 'b': pitchClass = 11; break;
            default: break;
        }
    }

    if (numChars > 1)
    {
        const auto sharpOrFlat = pitchClassName[1];

        switch (sharpOrFlat)
        {
            case '#': ++pitchClass; break;
            case 'b': --pitchClass; break;
            default: break;
        }

        if (sharpOrFlat == getSharpSymbol())
            ++pitchClass;
        else if (sharpOrFlat == getFlatSymbol())
            --pitchClass;

        pitchClass %= 12;
    }

    return pitchClass;
}

String Pitch::getValidPitchClassLetters()
{
    String chars ("abcdefg#b");
    chars << getSharpSymbol() << getFlatSymbol();
    return chars;
}
