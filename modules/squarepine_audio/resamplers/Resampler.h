/** Derive from this class to design a resampling system of your choosing! */
class Resampler
{
public:
    /** Constructor */
    Resampler() noexcept = default;

    /** Destructor */
    virtual ~Resampler() noexcept = default;

    /**  */
    virtual void prepare (int numChannels, double sampleRate, int numSamples) = 0;

    /**  */
    virtual void process (AudioBuffer<float>& source, AudioBuffer<float>& dest) = 0;

    /** Sets the ratio directly. */
    void setRatio (double newRatio)
    {
        if (ratio != newRatio && newRatio > 0.0)
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

    /** @returns the current resampling ratio */
    sp_nodiscard double getRatio() const noexcept { return ratio.load (std::memory_order_relaxed); }

    /** @returns the inverse resampling ratio */
    sp_nodiscard double getInverseRatio() const noexcept { return 1.0 / getRatio(); }

private:
    std::atomic<double> ratio { 1.0 };

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
    void process (AudioBuffer<float>& source, AudioBuffer<float>& dest) override
    {
        const auto r = getRatio();
        const auto numOutSamples = dest.getNumSamples();

        for (int i = jmin (source.getNumChannels(), dest.getNumChannels(), resamplers.size()); --i >= 0;)
            resamplers.getUnchecked (i)->process (r, source.getReadPointer (i), dest.getWritePointer (i), numOutSamples);
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
    void process (AudioBuffer<float>& source, AudioBuffer<float>& dest) override;
    /** @internal */
    void updateRatio() override;

private:
    OwnedArray<r8b::CDSPResampler24> resamplers;
    int numChannels = 0, blockSize = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (R8brainResampler)
};

#endif //SQUAREPINE_USE_R8BRAIN
