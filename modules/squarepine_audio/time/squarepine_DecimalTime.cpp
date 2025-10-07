DecimalTime::DecimalTime (double time) noexcept :
    timeSeconds (time)
{
    jassert (approximatelyEqual (timeSeconds, 0.0)
             || std::isnormal (timeSeconds));
}

DecimalTime::DecimalTime (int64 timeInSamples, double sampleRate) noexcept :
    DecimalTime (samplesToSeconds (timeInSamples, sampleRate))
{
}

DecimalTime::DecimalTime (int minutes, int seconds, int milliseconds) noexcept :
    DecimalTime (minutesToSeconds (minutes)
                 + seconds
                 + (double) (milliseconds / 1000))
{
}

DecimalTime::DecimalTime (int hours, int minutes, int seconds, int milliseconds) noexcept :
    DecimalTime (minutes, hoursToSeconds (hours) + seconds, milliseconds)
{
}

//==============================================================================
DecimalTime& DecimalTime::operator= (const DecimalTime& other) noexcept
{
    timeSeconds = other.timeSeconds;
    return *this;
}

bool DecimalTime::operator== (const DecimalTime& other) const noexcept
{
    return approximatelyEqual (timeSeconds, other.timeSeconds);
}

bool DecimalTime::operator!= (const DecimalTime& other) const noexcept
{
    return ! operator== (other);
}

//==============================================================================
constexpr int DecimalTime::minutesToSeconds (int minutes) noexcept
{
    return minutes > 0
            ? minutes * 60
            : -minutes * 60;
}

constexpr int DecimalTime::hoursToSeconds (int hours) noexcept
{
    return minutesToSeconds (hours * 60);
}

//==============================================================================
String DecimalTime::toString() const
{
    auto integral           = 0.0;
    auto fractional         = std::modf (timeSeconds, &integral);
    const auto t            = (int) integral;
    const auto numDays      = roundToInt ((double) timeSeconds / (double) (secondsPerHour * 24.0));
    const auto numHours     = (t % secondsPerHour) / secondsPerMinute / 24;
    const auto numMinutes   = (t % secondsPerHour) / secondsPerMinute;
    auto numSeconds         = (t % secondsPerHour) % (secondsPerMinute);
    const auto numMillis    = roundToInt (fractional * 1000.0);

    String min (numMinutes),
           sec (numSeconds),
           millis (numMillis);

    if (numMinutes < 10)
        min = "0" + String (numMinutes);

    if (numMillis < 10)
    {
        millis = "00" + String (numMillis);
    }
    else if (numMillis < 100)
    {
        millis = "0" + String (numMillis);
    }
    else if (numMillis == 1000)
    {
        millis = "000";
        numSeconds++;
    }

    if (numSeconds < 10)
        sec = "0" + String (numSeconds);

    if (numDays != 0)
    { //Include days and hours:
        String days (numDays), hrs (numHours);

        if (numDays < 10)
            days = "0" + String (numDays);

        if (numHours < 10)
            hrs = "0" + String (numHours);

        return days + ":" + hrs + ":" + min + ":" + sec + "." + millis;
    }

    if (numHours != 0)
    { //Include hours:
        String hrs (numHours);
        if (numHours < 10)
            hrs = "0" + String (numHours);

        return hrs + ":" + min + ":" + sec + "." + millis;
    }

    return min + ":" + sec + "." + millis;
}
