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

void HissingProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)     { process (buffer); }
void HissingProcessor::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&)    { process (buffer); }

template<typename FloatType>
void HissingProcessor::process (juce::AudioBuffer<FloatType>& buffer)
{
    auto resetParams = [this]()
    {
        blockCounter = 0;
        level = 0.001;
    };

    if (isBypassed())
    {
        resetParams();
        return;
    }

    constexpr auto hissLevel = 0.65;

    if (++blockCounter >= blocksBetweenHisses)
    {
        resetParams();
        blocksBetweenHisses = jmax (10, maxBlocksBetweenHisses - random.nextInt (maxBlocksBetweenHisses / 4));
    }

    if (blockCounter >= blocksPerHiss)
        return;

    for (auto channel : AudioBufferView (buffer))
    {
        for (auto& sample : channel)
        {
            auto noise = 0.0;

            for (int k = 4; --k >= 0;)
                noise += random.nextDouble();

            sample += (FloatType) (level * noise * 0.2);

            if (level < hissLevel)
                level *= 1.000025;
        }
    }
}
