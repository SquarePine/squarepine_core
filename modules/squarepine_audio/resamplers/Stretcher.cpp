Stretcher::Stretcher (const double sr,
                      const int ol,
                      const double stretch,
                      const double pitch) noexcept :
    sampleRate (sr),
    outputLength (ol),
    stretchFactor (stretch),
    pitchFactor (pitch)
{
    SQUAREPINE_CRASH_TRACER;

    jassert (ol > 0);
    jassert (sr > 0.0);

    stretchFactor = std::clamp (stretchFactor, 0.1, 10.0);
    pitchFactor = std::clamp (pitchFactor, 0.1, 10.0);
}

Stretcher::~Stretcher() noexcept
{
}

//==============================================================================
void Stretcher::setStretchAndPitch (double stretch, double pitch, bool exactStretch)
{
    setStretch (stretch);
    setPitch (pitch);

    update (exactStretch);
}

void Stretcher::setStretch (double stretch)
{
    if (stretch != stretchFactor)
    {
        stretchFactor = std::clamp (stretch, 0.1, 10.0);
        update (true);
    }
}

void Stretcher::setPitch (const double pitch)
{
    if (pitch != pitchFactor)
    {
        pitchFactor = std::clamp (pitch, 0.1, 10.0);
        update (false);
    }
}
