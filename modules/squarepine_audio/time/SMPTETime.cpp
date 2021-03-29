SMPTETime::SMPTETime (const int h, const int m, const int s, const int f, const double fr) noexcept :
    hours (h),
    minutes (m),
    seconds (s),
    frames (f),
    frameRate (fr)
{
    moveIntoRange();
}

SMPTETime::SMPTETime (const SMPTETime& smpteTime) noexcept :
    hours (smpteTime.hours),
    minutes (smpteTime.minutes),
    seconds (smpteTime.seconds),
    frames (smpteTime.frames),
    frameRate (smpteTime.frameRate)
{
    moveIntoRange();
}

SMPTETime::~SMPTETime() noexcept
{
}

void SMPTETime::moveIntoRange()
{
    if (frameRate < 24.0 || frameRate > 60.0)
        frameRate = toDouble (AudioPlayHead::fps60);

    if (frames > 60)
    {
        seconds = (frames / (uint8) roundToInt (frameRate));
        frames = (frames % (uint8) roundToInt (frameRate));
    }

    if (seconds > 60)
    {
        minutes = (seconds / 60);
        seconds += (seconds % 60);
    }

    if (minutes > 60)
    {
        hours += (minutes / 60);
        minutes += (minutes % 60);
    }
}

//==============================================================================
SMPTETime& SMPTETime::operator= (const SMPTETime& other) noexcept
{
    if (this != &other)
    {
        hours = other.hours;
        minutes = other.minutes;
        seconds = other.seconds;
        frames = other.frames;
        frameRate = other.frameRate;

        moveIntoRange();
    }

    return *this;
}

bool SMPTETime::operator== (const SMPTETime& other) const noexcept
{
    return hours == other.hours
           && minutes == other.minutes
           && seconds == other.seconds
           && frames == other.frames
           && frameRate == other.frameRate;
}

bool SMPTETime::operator!= (const SMPTETime& other) const noexcept
{
    return ! operator== (other);
}

//==============================================================================
double SMPTETime::toSeconds() const
{
    return (double) (hours * 3600.0)
         + (double) (minutes * 60.0)
         + (double) seconds
         + (double) (frames / frameRate);
}

//==============================================================================
SMPTETime SMPTETime::fromSeconds (double time, double frameRate) noexcept
{
    if (time >= 0.0)
    {
        auto integral = 0.0;
        const auto fractional = std::modf (time, &integral);
        const int t = (int) integral;

        SMPTETime smpte (0, 0, 0, 0, frameRate);
        smpte.hours     = (uint8) (t / secondsPerHour);
        smpte.minutes   = (uint8) ((t % secondsPerHour) / secondsPerMinute);
        smpte.seconds   = (uint8) ((t % secondsPerHour) % (secondsPerMinute));
        smpte.frameRate = frameRate;
        smpte.frames    = (uint8) (fractional * smpte.frameRate);

        return smpte;
    }

    jassertfalse;
    return {};
}

//==============================================================================
void SMPTETime::setFrameRate (const MidiMessage::SmpteTimecodeType rate) noexcept
{
    frameRate = toDouble (rate);
}

void SMPTETime::setFrameRate (const AudioPlayHead::FrameRateType rate) noexcept
{
    frameRate = toDouble (rate);
}

//==============================================================================
double SMPTETime::toDouble (const MidiMessage::SmpteTimecodeType rate) noexcept
{
    switch (rate)
    {
        case MidiMessage::fps24:        return 24.0;
        case MidiMessage::fps25:        return 25.0;
        case MidiMessage::fps30drop:    return 30.0 / 1.001;
        case MidiMessage::fps30:        return 30.0;

        default:
        break;
    };

    jassertfalse; //Unknown format!
    return 0.0;
}

double SMPTETime::toDouble (const AudioPlayHead::FrameRateType rate) noexcept
{
    switch (rate)
    {
        case AudioPlayHead::fps24:      return 24.0;
        case AudioPlayHead::fps25:      return 25.0;
        case AudioPlayHead::fps2997:    return 30.0 / 1.001;
        case AudioPlayHead::fps30:      return 30.0;
        case AudioPlayHead::fps30drop:  return 30.0 / 1.001;
        case AudioPlayHead::fps60:      return 60.0;
        case AudioPlayHead::fps60drop:  return 60.0 / 1.001;
        case AudioPlayHead::fpsUnknown: return 0.0;

        default:
        break;
    };

    jassertfalse; //Unknown format!
    return 0.0;
}

//==============================================================================
String SMPTETime::toString() const
{
    auto sec = String (seconds);
    if (seconds < 10)
        sec = "0" + sec;

    auto min = String (minutes);
    if (minutes < 10)
        min = "0" + min;

    auto hrs = String (hours);
    if (hours < 10)
        hrs = "0" + hrs;

    return hrs + ":" + min + ":" + sec;
}

String SMPTETime::toString (bool showFrames) const
{
    String fra, separator;

    if (showFrames)
    {
        fra = String (frames);
        if (frames < 10)
            fra = "0" + fra;

        auto integral = 0.0;
        std::modf (frameRate, &integral);
        separator = ! approximatelyEqual (integral, 0.0) ? "." : ":";
    }

    return toString() + separator + fra;
}

