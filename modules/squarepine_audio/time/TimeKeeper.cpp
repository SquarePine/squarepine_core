TimeKeeper::TimeKeeper (double sr)
{
    setSamplingRate (sr);
}

TimeKeeper::TimeKeeper (const AudioPlayHead::CurrentPositionInfo& info, double sr) :
    TimeKeeper (sr)
{
    timeSeconds = info.timeInSeconds;
    tempo = Tempo (info.bpm);
    timeSignature = TimeSignature (info.timeSigNumerator, info.timeSigDenominator);
    setFrameRate (info.frameRate.getType());
}

//==============================================================================
TimeKeeper& TimeKeeper::setSamplingRate (double rate)
{
    jassert (rate > 0.0); // Now how would that work?
    sampleRate = std::clamp (rate, 8000.0, 192000.0);
    return *this;
}

TimeKeeper& TimeKeeper::setSamplingRate (AudioDeviceManager& device)
{
    AudioDeviceManager::AudioDeviceSetup config;
    device.getAudioDeviceSetup (config);
    setSamplingRate (config.sampleRate);
    return *this;
}

//==============================================================================
TimeKeeper& TimeKeeper::setTime (double seconds)
{
    timeSeconds = jmax (0.0, seconds);
    return *this;
}

TimeKeeper& TimeKeeper::setTime (int64 samples)
{
    jassert (sampleRate > 0.0); // Did you forget to set the sample rate?

    timeSeconds = DecimalTime (samples, sampleRate).toSeconds();
    return *this;
}

TimeKeeper& TimeKeeper::setTime (int minutes, int seconds, int ms)
{
    timeSeconds = DecimalTime (minutes, seconds, ms).toSeconds();
    return *this;
}

TimeKeeper& TimeKeeper::setTime (int hours, int minutes, int seconds, int ms)
{
    timeSeconds = DecimalTime (hours, minutes, seconds, ms).toSeconds();
    return *this;
}

TimeKeeper& TimeKeeper::setTime (int hours, int minutes, int seconds, int frames,
                                 MidiMessage::SmpteTimecodeType rate)
{
    timeSeconds = SMPTETime (hours, minutes, seconds, frames, SMPTETime::toDouble (rate)).toSeconds();
    return *this;
}

TimeKeeper& TimeKeeper::setTime (int hours, int minutes, int seconds, int frames,
                                 AudioPlayHead::FrameRateType rate)
{
    timeSeconds = SMPTETime (hours, minutes, seconds, frames, SMPTETime::toDouble (rate)).toSeconds();
    return *this;
}

TimeKeeper& TimeKeeper::setFrameRate (MidiMessage::SmpteTimecodeType rate)
{
    frameRate = SMPTETime::toDouble (rate);
    return *this;
}

TimeKeeper& TimeKeeper::setFrameRate (AudioPlayHead::FrameRateType rate)
{
    frameRate = SMPTETime::toDouble (rate);
    return *this;
}

TimeKeeper& TimeKeeper::setTempo (const Tempo& t)
{
    tempo = t;
    return *this;
}

TimeKeeper& TimeKeeper::setTimeSignature (const TimeSignature& ts)
{
    timeSignature = ts;
    return *this;
}

//==============================================================================
TimeKeeper& TimeKeeper::setTimeFormat (TimeFormat format)
{
    timeFormat = format;
    return *this;
}

//==============================================================================
String TimeKeeper::toString() const
{
    switch (timeFormat)
    {
        case TimeFormat::smpteTime:   return SMPTETime::fromSeconds (timeSeconds, frameRate).toString (false);
        case TimeFormat::decimalTime: return DecimalTime (timeSeconds).toString();
        case TimeFormat::secondsTime: return { timeSeconds, 3 };

        case TimeFormat::samplesTime:
        {
            jassert (sampleRate > 0.0); // Did you forget to set the sample rate?

            return String (timeSecondsToSamples<int64> (timeSeconds, sampleRate));
        }
        break;

        case TimeFormat::measuresBeatsTicks:
        {
            auto whole = timeSeconds / timeSignature.getNumSecondsPerMeasure (tempo);
            auto beats = std::modf (whole, &whole);
            const auto ticks = std::modf (beats * timeSignature.numerator, &beats)
                                    * MBTTime::defaultTicksResolution;

            auto createString = [] (double val, bool needHundreds = false)
            {
                auto s = String (static_cast<int64> (val));
                s = val < 10.0 ? "0" + s : s;

                if (needHundreds && val < 100.0)
                    return "0" + s;

                return s;
            };

            return createString (whole, true) + ":"
                 + createString (beats, jmax (timeSignature.numerator, timeSignature.denominator) > 10) + ":"
                 + createString (ticks, true);
        }

        default:
            jassertfalse;
        break;
    };

    return {};
}
