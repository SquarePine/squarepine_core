#if SQUAREPINE_USE_ELASTIQUE

}

#include "elastiqueProV3API.h"

namespace sp
{

void logElastiqueError (int error)
{
    if (error != 0)
    {
        Logger::writeToLog ("Elastique failed instance creation. Returned code: " + String (error));
        jassertfalse;
    }
}

ElastiqueStretcher::ElastiqueStretcher (double sr,  int ol,
                                        double stretch, double pitch,
                                        bool exactStretch) noexcept :
    Stretcher (sr, ol, stretch, pitch)
{
    SQUAREPINE_CRASH_TRACER;

    logElastiqueError (CElastiqueProV3If::CreateInstance (elastiqueInstance, outputLength, 2, (float) sampleRate));

    reset (sampleRate, outputLength);
    update (exactStretch);
    internalBuffer.clear();
}

ElastiqueStretcher::~ElastiqueStretcher()
{
    SQUAREPINE_CRASH_TRACER;

    CElastiqueProV3If::DestroyInstance (elastiqueInstance);
}

int ElastiqueStretcher::getInputLength() const
{
    return elastiqueInstance->GetFramesNeeded();
}

int ElastiqueStretcher::getInputLength (int newOutputLength)
{
    SQUAREPINE_CRASH_TRACER;

    jassert (newOutputLength >= 0);
    outputLength = newOutputLength;
    return elastiqueInstance->GetFramesNeeded (outputLength);
}

int ElastiqueStretcher::getMaxInputLength() const
{
    return elastiqueInstance->GetMaxFramesNeeded();
}

void ElastiqueStretcher::reset (double newSampleRate, int newOutputLength)
{
    SQUAREPINE_CRASH_TRACER;

    if (newSampleRate != sampleRate)
    {
        sampleRate = newSampleRate;
        outputLength = newOutputLength;
        CElastiqueProV3If::DestroyInstance (elastiqueInstance);

        logElastiqueError (CElastiqueProV3If::CreateInstance (elastiqueInstance, outputLength, 2, (float) sampleRate, CElastiqueProV3If::kV3mobile));
    }
    else
    {
        outputLength = newOutputLength;
        elastiqueInstance->GetFramesNeeded (outputLength);
        elastiqueInstance->Reset();
    }

    update (true);

    internalBuffer.setSize (2, elastiqueInstance->GetMaxFramesNeeded(), false, true, true);
    internalBuffer.clear();
}

void ElastiqueStretcher::update (const bool exactStretch)
{
    if (exactStretch)
    {
        auto pitch = (float) pitchFactor;
        elastiqueInstance->SetStretchPitchQFactor ((float) stretchFactor, pitch);
        pitchFactor = (double) pitch;
    }
    else
    {
        auto stretch = (float) stretchFactor;
        elastiqueInstance->SetStretchQPitchFactor (stretch, (float) pitchFactor);
        stretchFactor = (double) stretch;
    }
}

void ElastiqueStretcher::process (AudioBuffer<float>& buffer)
{
    SQUAREPINE_CRASH_TRACER;

    for (int i = 0; i < buffer.getNumChannels(); ++i)
        internalBuffer.copyFrom (i, 0, buffer, i, 0, getInputLength());

    buffer.clear();

    logElastiqueError (elastiqueInstance->ProcessData (internalBuffer.getArrayOfWritePointers(), getInputLength(), buffer.getArrayOfWritePointers()));
}

int ElastiqueStretcher::getRemainingSamples (AudioBuffer<float>& buffer)
{
    const int numOfSamples = elastiqueInstance->GetFramesBuffered();
    elastiqueInstance->FlushBuffer (buffer.getArrayOfWritePointers());
    return numOfSamples;
}

#endif // SQUAREPINE_USE_ELASTIQUE
