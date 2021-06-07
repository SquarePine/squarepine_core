TimeSignature::TimeSignature (const int num, const int den) noexcept :
    numerator (num),
    denominator (den)
{
    snapToRange();
}

//==============================================================================
TimeSignature TimeSignature::withNumerator (int n) const
{
    auto o = *this;
    o.numerator = n;
    o.snapToRange();
    return o;
}

TimeSignature TimeSignature::withDenominator (int d) const
{
    auto o = *this;
    o.denominator = d;
    o.snapToRange();
    return o;
}

//==============================================================================
bool TimeSignature::operator== (const TimeSignature& other) const noexcept  { return numerator == other.numerator && denominator == other.denominator; }
bool TimeSignature::operator!= (const TimeSignature& other) const noexcept  { return ! operator== (other); }
bool TimeSignature::operator< (const TimeSignature& other) const noexcept   { return numerator < other.numerator && denominator < other.denominator; }
bool TimeSignature::operator<= (const TimeSignature& other) const noexcept  { return numerator <= other.numerator && denominator <= other.denominator; }
bool TimeSignature::operator> (const TimeSignature& other) const noexcept   { return numerator > other.numerator && denominator > other.denominator; }
bool TimeSignature::operator>= (const TimeSignature& other) const noexcept  { return numerator >= other.numerator && denominator >= other.denominator; }

//==============================================================================
TimeSignature TimeSignature::getTimeSignature (const MidiFile& midiFile)
{
    for (int i = 0; i < midiFile.getNumTracks(); ++i)
    {
        auto track = *midiFile.getTrack (i);
        track.sort();

        for (auto* meh : track)
        {
            const auto& msg = meh->message;
            if (msg.isTimeSignatureMetaEvent())
            {
                int n = 0, d = 0;
                msg.getTimeSignatureInfo (n, d);
                return { n, d };
            }
        }
    }

    return {};
}

TimeSignature TimeSignature::getTimeSignature (const AudioFormatReader& reader)
{
    TimeSignature timeSig;

    const auto& metadata = reader.metadataValues;

    if (reader.getFormatName().containsIgnoreCase ("WAV"))
    {
        if (metadata.containsKey (WavAudioFormat::acidNumerator))   timeSig.numerator = metadata[WavAudioFormat::acidNumerator].getIntValue();
        if (metadata.containsKey (WavAudioFormat::acidDenominator)) timeSig.denominator = metadata[WavAudioFormat::acidDenominator].getIntValue();
    }
   #if SQUAREPINE_USE_REX_AUDIO_FORMAT
    else if (reader.getFormatName().containsIgnoreCase ("REX"))
    {
        if (metadata.containsKey (REXAudioFormat::rexNumerator))    timeSig.numerator = metadata[REXAudioFormat::rexNumerator].getIntValue();
        if (metadata.containsKey (REXAudioFormat::rexDenominator))  timeSig.denominator = metadata[REXAudioFormat::rexDenominator].getIntValue();
    }
   #endif
   #if JUCE_MAC
    else if (reader.getFormatName().containsIgnoreCase ("CoreAudio"))
    {
        if (metadata.containsKey (CoreAudioFormat::timeSig))        timeSig = TimeSignature::fromString (metadata[CoreAudioFormat::timeSig]);
    }
   #endif

    return timeSig;
}

//==============================================================================
void TimeSignature::snapToRange() noexcept
{
    numerator = jlimit (mininumNumerator, maximumNumerator, numerator);

    if (! isPowerOfTwo (denominator))
        denominator = previousPowerOfTwo (denominator);

    denominator = jlimit (mininumDenominator, maximumDenominator, denominator);
}

//==============================================================================3
double TimeSignature::getNumQuarterNotesPerMeasure() const noexcept
{
    return 4.0 / (double) numerator * (double) denominator;
}

double TimeSignature::getNumSecondsPerMeasure (double tempo) const noexcept
{
    return determineSecondsPerMeasure (getNumQuarterNotesPerMeasure(), tempo);
}

double TimeSignature::getNumSecondsPerMeasure (const Tempo& tempo) const noexcept
{
    return getNumSecondsPerMeasure (tempo.get());
}

//==============================================================================
String TimeSignature::toString() const
{
    return String (numerator) + ":" + String (denominator);
}

TimeSignature TimeSignature::fromString (const String& s)
{
    const auto n = s.upToFirstOccurrenceOf (":", false, true).trim().getIntValue();
    const auto d = s.fromLastOccurrenceOf (":", false, true).trim().getIntValue();

    return { n, d };
}
