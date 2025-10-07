MBTTime::MBTTime (const int m, const int b, const int t) :
    measure (m),
    beat (b),
    tick (t)
{
}

MBTTime::MBTTime (const double ticks, const int ppq, const Array<TimeSignature>& timeSigs, const double songLength)
{
    MBTTime::ticksToMBTTime (*this, ticks, ppq, timeSigs, songLength);
}

MBTTime::MBTTime (const MBTTime& mbt) :
    measure (mbt.measure),
    beat (mbt.beat),
    tick (mbt.tick)
{
}

MBTTime::~MBTTime()
{
}

//==============================================================================
MBTTime& MBTTime::operator= (const MBTTime& other)
{
    measure = other.measure;
    beat = other.beat;
    tick = other.tick;

    return *this;
}

bool MBTTime::operator== (const MBTTime& other) const
{
    return measure == other.measure
           && beat == other.beat
           && tick == other.tick;
}

bool MBTTime::operator!= (const MBTTime& other) const
{
    return ! operator== (other);
}

//==============================================================================
void MBTTime::ticksToMBTTime (MBTTime&, double, int, const Array<TimeSignature>&, double)
{
#if 0
    jassert (ticks >= 0);        //Our MIDI setup is not meant to play in a different spacetime continuum...
    jassert (ppq > 0);           //Invalid PPQ! You might as well try to figure out how to divide by 0!
    jassert (songLength >= 0);   //A song's length cannot be negative!

    if ((ticks < 0) || (ppq <= 0) || (songLength < 0))
    {
        out = MBTTime();
        return;
    }

    auto timeDelta = 0.0;
    int i;

    for (i = 0; i < timeSigs.size(); ++i)
    {
        const auto tsATime = timeSigs.getUnchecked (i).timestamp;

        double tsBTime = songLength;
        if ((i + 1) < timeSigs.size())
            tsBTime = timeSigs[i + 1].timestamp;

        if ((ticks >= tsATime) && (ticks < tsBTime))
        {
            timeDelta = tsBTime - tsATime;
            break;
        }
    }

    const auto timeSig = timeSigs.getUnchecked (i);

    double integral = 0;
    const auto fractional   = std::modf ((double) (ticks / ppq), &integral);
    const auto barLength    = determineBarLengthInTicks (timeSig.numerator, timeSig.denominator, (short) ppq);
    const auto delta        = (ticks - timeSig.timestamp);

    out.tick    = (int) std::round (fractional * (double) ppq);
    out.beat    = ((int) (delta / (barLength / timeSig.denominator)) % timeSig.denominator) + 1;
    out.measure = (int) std::round ((delta / barLength) + 1.0);
#endif
}

//==============================================================================
double MBTTime::toTicks (const Array<TimeSignature>&, int)
{
    return 0.0;
}

//==============================================================================
String MBTTime::toString() const
{
    String text (measure);

    if (measure < 10)
        text = "00" + text;
    else if (measure < 100 && measure > 10)
        text = "0" + text;

    text += ":";

    if (beat < 10)
        text += "0" + String (beat);
    else
        text += String (beat);

    text += ":";

    if (tick < 10)
        text += "00" + String (tick);
    else if (tick < 100 && tick > 10)
        text += "0" + String (tick);
    else
        text += String (tick);

    return text;
}

double MBTTime::toSeconds() const
{
    return 0.0;
}
