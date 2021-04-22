void HissingProcessor::prepareToPlay (const double newSampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);

    constexpr auto secondsBetweenHisses = 3;

    maxBlocksBetweenHisses = blocksBetweenHisses
        = jmax (10, roundToInt ((newSampleRate * (double) (random.nextInt (10) + secondsBetweenHisses))
                                / (double) estimatedSamplesPerBlock));

    blockCounter = random.nextInt (blocksBetweenHisses);
    blocksPerHiss = jmax (2, roundToInt (newSampleRate * 2.5 / estimatedSamplesPerBlock));
    level = 0.001;
}

void HissingProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)
{
    if (isBypassed())
        return;

    constexpr auto hissLevel = 0.65;

    if (++blockCounter >= blocksBetweenHisses)
    {
        blockCounter = 0;
        level = 0.001;

        blocksBetweenHisses = jmax (10, maxBlocksBetweenHisses - random.nextInt (maxBlocksBetweenHisses / 4));
    }

    if (blockCounter < blocksPerHiss)
    {
        for (int j = 0; j < buffer.getNumChannels(); ++j)
        {
            auto* d = buffer.getWritePointer (j, 0);

            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                auto noise = 0.0;

                for (int k = 4; --k >= 0;)
                    noise += random.nextDouble();

                *d++ += (float) (level * noise * 0.2);

                if (level < hissLevel)
                    level *= 1.000025;
            }
        }
    }
}
