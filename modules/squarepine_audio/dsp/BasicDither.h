/** This is a basic 1st order noise-shaping dither. */
class BasicDither final
{
public:
    BasicDither() noexcept { reset(); }

    //==============================================================================
    void reset() noexcept
    {
        generator.setSeedRandomly();

        wordLen = std::pow (2.0f, 31.0f);
        invWordLen = 1.0f / wordLen;
        amp = invWordLen / (float) RAND_MAX;
        offset = invWordLen / 2.0f;
    }

    float generateNextSample (float inputSample) const noexcept
    {
        return inputSample + offset + amp * (float) (random1 - random2);
    }

    void processAdditiveDither (float& inputSample) const noexcept
    {
        inputSample += 0.5f * (s1 + s1 - s2);
    }

    void process (float& inputSample, float& outSample) noexcept
    {
        processAdditiveDither (inputSample);
        outSample = generateNextSample (inputSample);
    }

    void process (float* channel, int numSamples) noexcept
    {
        if (channel == nullptr || numSamples <= 0)
            return;

        while (--numSamples >= 0)
        {
            random2 = random1;

            //N.B.: This was using 'rand()', but that C function isn't multithreaded FFS!
            random1 = generator.nextInt (RAND_MAX);

            auto in = *channel;
            float out;

            //Check for dodgy numbers coming in:
            if (in < -0.000001f || in > 0.000001f)
            {
                process (in, out);
                *channel++ = out;
            }
            else
            {
                *channel++ = in;
                process (in, out);
            }

            s2 = s1;
            s1 = in - out;
        }
    }

private:
    //==============================================================================
    Random generator; // The distribution of this Random class is questionable...
    int random1 = 0, random2 = 0;
    float wordLen = 0.0f, invWordLen = 0.0f, amp = 0.0f, offset = 0.0f;
    float s1 = 0.0f, s2 = 0.0f;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicDither)
};

