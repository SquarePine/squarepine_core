//==============================================================================
#if SQUAREPINE_USE_R8BRAIN

void R8brainResampler::prepare (int channels, int numSamples, double)
{
    numChannels = channels;
    blockSize = numSamples;

    updateRatio();
}

void R8brainResampler::process (juce::AudioBuffer<float>& source, juce::AudioBuffer<float>& dest)
{
    const auto r = getRatio();
    const auto numInSamples = source.getNumSamples();
    const auto numOutSamples = dest.getNumSamples();

    for (int channel = 0; channel < source.getNumChannels(); channel++)
        resamplers[channel]->oneshot (numInSamples, source.getReadPointer (channel), numInSamples,
                                      dest.getWritePointer (channel), numOutSamples);
}

void R8brainResampler::updateRatio()
{
    const auto srcRatio = getRatio();

    if (srcRatio != 1.0 && srcRatio > 0.0)
    {
        resamplers.clearQuick (true);

        for (int i = 0; i < numChannels; ++i)
            resamplers.add (new r8b::CDSPResampler24 (1, srcRatio, blockSize));
    }
}

#endif
