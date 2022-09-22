/** Derive from this class to design a resampling system of your choosing! */
class Resampler
{
public:
    /** Constructor. */
    Resampler() noexcept = default;

    /** Destructor. */
    virtual ~Resampler() noexcept = default;

    //==============================================================================
    /**  */
    virtual void prepare (int numChannels, double sampleRate, int numSamples) = 0;

    /**  */
    virtual void configure (int numResultingSamples, int numChannels,
                            juce::AudioBuffer<float>& source,
                            juce::AudioBuffer<float>& destination)
    {
        const auto numSamples = roundToIntAccurate (getRatio() * numResultingSamples);

        source.setSize (numChannels, numSamples, false, false, true);
        source.clear();

        destination.setSize (numChannels, numResultingSamples, false, false, true);
        destination.clear();
    }

    /** Resamples a stream of samples.

        @param source       The source data to read from. This must contain at
                            least (speedRatio * numOutputSamplesToProduce) samples.
        @param destination  The buffer to write the results into.

        @returns the actual number of input samples that were used
    */
    virtual int process (juce::AudioBuffer<float>& source,
                         juce::AudioBuffer<float>& destination) = 0;

    //==============================================================================
    /** Sets the ratio directly. */
    void setRatio (double newRatio)
    {
        if (! approximatelyEqual (ratio.load(), newRatio) && newRatio > 0.0)
        {
            ratio = newRatio;
            updateRatio();
        }
    }

    /** Set the ratio via sampling rates; the source rate, and the destination rate to correct to. */
    void setRatio (double sourceRate, double destinationRate)
    {
        if (sourceRate > 0.0 && destinationRate > 0.0)
            setRatio (sourceRate / destinationRate);
    }

    /** If the ratio has changed, you might need to override this to update your subclass. */
    virtual void updateRatio() {}

    /** @returns the current resampling ratio. */
    [[nodiscard]] double getRatio() const noexcept { return ratio.load(); }

    /** @returns the inverse resampling ratio. */
    [[nodiscard]] double getInverseRatio() const noexcept { return 1.0 / getRatio(); }

private:
    //==============================================================================
    std::atomic<double> ratio { 1.0 };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Resampler)
};

//==============================================================================
/** */
template<typename ResamplerType>
class TemplatedResampler final : public Resampler
{
public:
    /** Default Constructor. */
    TemplatedResampler() noexcept = default;

    /** Call this to prepare this fairly straightforward resampler. */
    void prepare (int numChannels)
    {
        resamplers.ensureStorageAllocated (numChannels);

        for (int i = 0; i < numChannels; ++i)
        {
            if (auto* r = resamplers[i])
                r->reset();
            else
                resamplers.add (new ResamplerType());
        }
    }

    /** @internal */
    void prepare (int numChannels, double sampleRate, int numSamples) override
    {
        ignoreUnused (sampleRate, numSamples);
        prepare (numChannels);
    }

    /** @internal */
    int process (juce::AudioBuffer<float>& source, juce::AudioBuffer<float>& dest) override
    {
        const auto localRatio = getRatio();

        if (approximatelyEqual (localRatio, 1.0) || source.hasBeenCleared())
        {
            dest = source;
            return source.getNumSamples();
        }

        int numSamplesUsed = 0;
        const auto numOutSamples = dest.getNumSamples();
        const auto numChans = jmin (source.getNumChannels(), dest.getNumChannels(), resamplers.size());
        if (numChans <= 0)
            dest.clear();

        for (int i = numChans; --i >= 0;)
            numSamplesUsed = jmax (numSamplesUsed,
                                   resamplers.getUnchecked (i)->process (localRatio,
                                                                         source.getReadPointer (i),
                                                                         dest.getWritePointer (i),
                                                                         numOutSamples));

        return numSamplesUsed;
    }

private:
    //==============================================================================
    OwnedArray<ResamplerType> resamplers; // Need to have one resampler per channel.

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TemplatedResampler)
};

//==============================================================================
/** */
using WindowedSincResampler = TemplatedResampler<WindowedSincInterpolator>;
/** */
using LagrangeResampler = TemplatedResampler<LagrangeInterpolator>;
/** */
using CatmullRomResampler = TemplatedResampler<CatmullRomInterpolator>;
/** */
using LinearResampler = TemplatedResampler<LinearInterpolator>;
/** */
using ZeroOrderHoldResampler = TemplatedResampler<ZeroOrderHoldInterpolator>;

//==============================================================================
#if SQUAREPINE_USE_R8BRAIN

/** */
class R8brainResampler final : public Resampler
{
public:
    /** Constructor. */
    R8brainResampler() noexcept = default;

    /** @internal */
    void prepare (int numChannels, int numSamples, double sampleRate) override;
    /** @internal */
    int process (juce::AudioBuffer<float>& source, juce::AudioBuffer<float>& dest) override;
    /** @internal */
    void updateRatio() override;

private:
    OwnedArray<r8b::CDSPResampler24> resamplers;
    int numChannels = 0, blockSize = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (R8brainResampler)
};

#endif //SQUAREPINE_USE_R8BRAIN
