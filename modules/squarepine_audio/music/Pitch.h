/** This class contains some useful methods for storing and converting different
    representations of a pitch.
*/
class Pitch final
{
public:
    //==============================================================================
    /** Create a default pitch with a frequency of 0 Hz. */
    Pitch() noexcept = default;

    /** Create a pitch with a given frequency in Hz. */
    Pitch (double frequencyHz) noexcept;

    /** Creates a copy of another pitch. */
    Pitch (const Pitch&) noexcept = default;

    /** Moves the frequency of another pitch into this one. */
    Pitch (Pitch&&) noexcept = default;

    /** Copies the frequency of another pitch into this one. */
    Pitch& operator= (const Pitch&) noexcept = default;

    /** Moves the frequency of another pitch into this one. */
    Pitch& operator= (Pitch&&) noexcept = default;

    /** Destructor. */
    ~Pitch() noexcept = default;

    //==============================================================================
    /** Creates a pitch from a given MIDI note number e.g. 69. */
    static Pitch fromMIDINote (int midiNote) noexcept       { return midiNoteToFrequency (midiNote); }

    /** Creates a pitch from a given note name e.g. A#3.

        This should be the pitch class followed by the octave.

        The pitch class can contain sharps and flats in the
        form of either #, b, or the Unicode character equivalents.

        @returns a valid pitch if found, or a Pitch with a frequency of 0.0 on failure.

        @see getSharpSymbol, getFlatSymbol
     */
    static Pitch fromNoteName (const String& noteName);

    //==============================================================================
    /** @returns the frequency of the pitch in Hz. */
    [[nodiscard]] double getFrequencyHz() const noexcept    { return frequency; }

    /** @returns the MIDI note of the pitch. e.g. 440 = 69. */
    [[nodiscard]] int getMIDINote() const noexcept          { return frequencyToMIDINote (frequency); }

    /** @returns the note name of the pitch. e.g. 440 = A4. */
    [[nodiscard]] String getMIDINoteName() const;

    //==============================================================================
    /** @returns a Unicode sharp symbol.

        @warning Not all fonts support this glyph, so be careful
                 when using this for display purposes!
    */
    static juce_wchar getSharpSymbol() noexcept   { return *CharPointer_UTF8 ("\xe2\x99\xaf"); }

    /** @returns a Unicode flat symbol.

        @warning Not all fonts support this glyph, so be careful
                 when using this for display purposes!
    */
    static juce_wchar getFlatSymbol() noexcept    { return *CharPointer_UTF8 ("\xe2\x99\xad"); }

    /** @returns a Unicode natural symbol.

        @warning Not all fonts support this glyph, so be careful
                 when using this for display purposes!
    */
    static juce_wchar getNaturalSymbol() noexcept { return *CharPointer_UTF8 ("\xe2\x99\xae"); }

private:
    //==============================================================================
    double frequency = 0.0;

    //==============================================================================
    /** @returns a pitch class number in the range of 0 - 12 to a letter. */
    static String getNoteName (int pitchClass, bool asSharps);

    /** @returns the pitch class number for a given string.
        If the string is not in the required format, eg: A#4, this will return -1.
    */
    static int getPitchClass (const String& pitchClassName);

    /** @returns the letters valid for a pitch class. */
    static String getValidPitchClassLetters();
};

//==============================================================================
/** */
inline String getPitchAsString (int pitch, int octaveNumForMiddleC)
{
    const auto sharp = MidiMessage::getMidiNoteName (pitch, true, false, octaveNumForMiddleC);
    const auto flat = MidiMessage::getMidiNoteName (pitch, false, false, octaveNumForMiddleC);

    if (sharp == flat)
        return sharp;

    return sharp + " / " + flat;
}

/** */
inline StringArray getPitchAsStrings (int pitch, int octaveNumForMiddleC)
{
    const auto sharp = MidiMessage::getMidiNoteName (pitch, true, false, octaveNumForMiddleC);
    const auto flat = MidiMessage::getMidiNoteName (pitch, false, false, octaveNumForMiddleC);

    StringArray res;

    if (sharp == flat)
    {
        res.add (sharp);
        return res;
    }

    res.add (sharp);
    res.add (flat);
    res.add (sharp + " / " + flat);

    return res;
}

/** */
inline int getPitchFromString (const String& str, int octaveNumForMiddleC)
{
    for (int i = 0; i < 12; ++i)
        if (getPitchAsStrings (i + 60, octaveNumForMiddleC).contains (str))
            return i + 60;

    return 60;
}

/** */
inline StringArray getPitchStrings (bool separateSharpFlat, int octaveNumForMiddleC)
{
    StringArray pitchChoices;

    if (separateSharpFlat)
    {
        for (int i = 0; i < 12; ++i)
            for (const auto& s : getPitchAsStrings (i + 60, octaveNumForMiddleC))
                if (! s.contains ("/"))
                    pitchChoices.add (s);
    }
    else
    {
        for (int i = 0; i < 12; ++i)
            pitchChoices.add (getPitchAsString (i + 60, octaveNumForMiddleC));
    }

    return pitchChoices;
}

/** @returns a relative number of semitones up or down converted into a speed ratio, where 0 semitones = 1.0. */
inline double semitonesToRatio (double semitonesUp) noexcept
{
    if (approximatelyEqual (semitonesUp, 0.0))
        return 1.0;

    const auto oneSemitone = std::pow (2.0, 1.0 / 12.0);
    return std::pow (oneSemitone, semitonesUp);
}
