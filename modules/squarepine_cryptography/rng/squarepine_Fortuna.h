/** A stateful pseudo-random number encompassing the Fortuna algorithm.

    @see https://en.wikipedia.org/wiki/Fortuna_(PRNG)
*/
class Fortuna final
{
public:
    Fortuna()
    {
        reseed();
    }

    Fortuna (uint64 seed)
    {
    }

    //==============================================================================
    void combineSeed (uint64 value)
    {
    }

    void combineSeed (const MemoryBlock& bytesToParse)
    {
        for (auto b : bytesToParse)
            combineSeed ((uint64) b);
    }

    void reseed()
    {
        combineSeed (SHA256 (String (__FILE__)).getRawData());
        combineSeed (SHA256 (String (__DATE__)).getRawData());
        combineSeed ((uint64) (pointer_sized_int) this);
        combineSeed ((uint64) Time::getMillisecondCounter());
        combineSeed ((uint64) Time::getHighResolutionTicks());
        combineSeed ((uint64) Time::getHighResolutionTicksPerSecond());
        combineSeed ((uint64) Time::currentTimeMillis());
    }

    //==============================================================================
    /** */
    class EntropyGenerator
    {
    public:
        /** */
        EntropyGenerator() noexcept = default;
        /** */
        virtual ~EntropyGenerator() noexcept = default;

        /** */
        virtual uint64 generateEntropy() = 0;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EntropyGenerator)
    };

    void addGenerator (std::unique_ptr<EntropyGenerator> generator)
    {
        jassert (generator != nullptr);

        if (generator != nullptr)
        {
        }
    }

    //==============================================================================
    /** @returns a new random value, between 1 all the way to ULLONG_MAX, in theory. */
    uint64 generate()
    {
        return 0;
    }

    /** @returns a new random value, where 0 < value <= ULLONG_MAX,
        albeit normalised between 0 < value <= 1.0.
    */
    double generateNormalised()
    {
        return 0.0;
    }

private:
    //==============================================================================
    using Pool  = OwnedArray<EntropyGenerator>;
    using Pools = OwnedArray<Pool>;

    std::unique_ptr<Threefish> cipher;
    Pools pools;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Fortuna)
};
