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
void TimeSignature::snapToRange()
{
    numerator = std::clamp (numerator, mininumNumerator, maximumNumerator);

    if (! isPowerOfTwo (denominator))
        denominator = previousPowerOfTwo (denominator);

    denominator = std::clamp (denominator, mininumDenominator, maximumDenominator);
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
    return getNumSecondsPerMeasure (tempo.value);
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
