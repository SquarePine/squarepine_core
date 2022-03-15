//==============================================================================
#if 0 && ! DOXYGEN
    //These are some bits of code that could be useful someday...

    /** Obtain the maximum value, pre-negative a number of bits to the */
    inline float getMaxValueForNumBits (int numBits) noexcept
    {
        BigInteger biggy;
        biggy.setRange (0, numBits - 1, true);

        return (float) biggy.toInteger() * 0.5f; //Halved to offset, making the middle-most number 0 dBFS
    }

    /** Discrete sample bit-conversion algorithm */
    inline float reduceSample (float sample, int bitDepth) noexcept
    {
        return (float) std::clamp (getMaxValueForNumBits (bitDepth) * sample, -maxValue, maxValue);
    }

    inline constexpr float compressSample (float sample, float scale) noexcept
    {
        return sample * scale;
    }

    // Ye olde...
    m = (float) (1 << (localBitDepth - 1));
    M = 1.0f / (float) m;

    for (int i = numChannels; --i >= 0;)
        for (int f = numSamples; --f >= 0;)
            dest[i][f] = (float) ((float) src[i][f] * m * M);
#endif

//==============================================================================
inline double crushToNBit (double sample, int bitDepth)
{
    constexpr auto one = 1.0;
    constexpr auto two = 2.0;

    const auto bd = bitDepth;
    auto s = 0.0;

	if (sample >= 1.0f)
       s = std::pow (two, bd - 1) - one;
	else if (sample <= -1.0f)
       s = std::pow (-two, bd - 1);
    else
       s = std::floor (sample * -std::pow (-two, bd - one));

    // NB: Deliberately quantising with casts here!
	return (double) (int) s;
}

inline double crushBit (double sample, int bitDepth)
{
/*
    auto mixAmount = lerp (1.0, 32.0, static_cast<double> (bitDepth) / 32.0) / 32.0;

    mixAmount *= 0.08;

    mixAmount = 0.0;

    sample = (sample * (1.0 - mixAmount))
           + (DistortionFunctions::hyperbolicTangentSoftClipping (sample) * mixAmount);
*/
    sample = crushToNBit (sample, bitDepth);

	sample = sample / -std::pow (-2.0, (double) bitDepth - 1.0);

    return sample;
}

//==============================================================================
BitCrusherProcessor::BitCrusherProcessor()
{
    AudioProcessor::addParameter (bitDepth);
}

//==============================================================================
void BitCrusherProcessor::setBitDepth (int newBitDepth)
{
    bitDepth->operator= (newBitDepth);
}

int BitCrusherProcessor::getBitDepth() const noexcept
{
    return bitDepth->get();
}

//==============================================================================
void BitCrusherProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    int localBitDepth = 32;

    {
        const ScopedLock sl (getCallbackLock());
        localBitDepth = bitDepth->get();
    }

    if (buffer.hasBeenCleared() || localBitDepth >= 32)
        return; // Nothing to do here.

    for (auto channel : AudioBufferView<float> (buffer))
        for (auto& sample : channel)
            sample = (float) crushBit ((double) sample, localBitDepth);
}
