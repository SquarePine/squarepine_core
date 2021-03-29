DecimalTime::DecimalTime (double time) noexcept :
    timeInSeconds (time)
{
}

DecimalTime::DecimalTime (const int64 timeInSamples, const double sampleRate) noexcept :
    timeInSeconds (timeSamplesToSeconds (timeInSamples, sampleRate))
{
}

DecimalTime::DecimalTime (const int minutes, const int seconds, const int milliseconds) noexcept :
    timeInSeconds ((double) ((milliseconds / 1000) + seconds + (minutes * 60)))
{
}

DecimalTime::DecimalTime (const int hours, const int minutes, const int seconds, const int milliseconds) noexcept :
    timeInSeconds (DecimalTime (minutes + (hours * 60), seconds, milliseconds).toSeconds())
{
}

//==============================================================================
DecimalTime& DecimalTime::operator= (const DecimalTime& other) noexcept
{
    timeInSeconds = other.timeInSeconds;
    return *this;
}

bool DecimalTime::operator== (const DecimalTime& other) const noexcept
{
    return timeInSeconds == other.timeInSeconds;
}

bool DecimalTime::operator!= (const DecimalTime& other) const noexcept
{
    return ! operator== (other);
}

//==============================================================================
int DecimalTime::hoursToSeconds (const int hours) noexcept
{
    return DecimalTime::minutesToSeconds (hours * 24);
}

int DecimalTime::minutesToSeconds (const int minutes) noexcept
{
    return std::abs (minutes) * 60;
}

//==============================================================================
double DecimalTime::toSeconds() const
{
    return timeInSeconds;
}

String DecimalTime::toString() const
{
    auto integral = 0.0;
    auto fractional = std::modf (timeInSeconds, &integral);
    const int t = (int) integral;

    const int numDays       = roundToInt ((double) timeInSeconds / (double) (secondsPerHour * 24.0));
    const int numHours      = (t % secondsPerHour) / secondsPerMinute / 24;
    const int numMinutes    = (t % secondsPerHour) / secondsPerMinute;
    int numSeconds          = (t % secondsPerHour) % (secondsPerMinute);
    const int numMillis     = roundToInt (fractional * 1000.0);

    String min (numMinutes);
    String sec (numSeconds);
    String millis (numMillis);

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
        String days (numDays);
        String hrs (numHours);

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
