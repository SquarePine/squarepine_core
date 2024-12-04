//==============================================================================
/** A stateful, pseudo-random number generator encompassing the Xorshift algorithm.

    Based on multiple sources, the Xorshift algorithm is a modern
    alternative to PSRGs like the Mersenne-Twister, having better
    randomness and improved speed.

    This is a stateful PSRG, therefore keeping one seeded instance around
    to continuously generate a random number is suggested. That being said,
    this is in no way thread-safe.

    @warning This algorithm, like the Mersenne-Twiser,
             is not cryptographically secure.

    @see Xorshift64

    @see https://en.wikipedia.org/wiki/Xorshift
    @see https://www.jstatsoft.org/article/view/v008i14/xorshift.pdf
    @see http://saluc.engr.uconn.edu/refs/crypto/rng/panneton05onthexorshift.pdf
*/
class Xorshift32 final
{
public:
    /** Constructor that preseeds the system with the current time. */
    Xorshift32();

    /** Constructor that takes a custom initial seed. */
    Xorshift32 (uint32 seedToStartWith);

    //==============================================================================
    /** @returns */
    [[nodiscard]] uint32 generate();

    /** @returns a new random value between 0 and 1. */
    [[nodiscard]] float generateNormalised();

private:
    //==============================================================================
    uint32 state = 2463534242U;
    const NormalisableRange<float> normaliser { 0.0f, (float) std::numeric_limits<uint32>::max() - 1 };

    //==============================================================================
    static uint32 xorshift (uint32&) noexcept;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Xorshift32)
};

//==============================================================================
/** A variant of Xorshift32 but for generating 64-bit values. */
class Xorshift64 final
{
public:
    /** */
    enum class Algorithm
    {
        standard,
        star,
        xorshift128p,
        xoshiro256pp,
        xoshiro256ss,
        xoshiro256p,
        xorshift1024s
    };

    //==============================================================================
    /** Constructor that preseeds the system with the current time. */
    Xorshift64 (Algorithm algorithmToStartWith = Algorithm::standard);

    /** Constructor that takes a custom initial seed. */
    Xorshift64 (uint64 seedToStartWith,
                Algorithm algorithmToStartWith = Algorithm::standard);

    //==============================================================================
    /** */
    void setAlgorithm (Algorithm newAlgorithmToChoose);
    /** */
    [[nodiscard]] Algorithm getAlgorithm() const noexcept { return algorithm; }

    //==============================================================================
    /** @returns */
    [[nodiscard]] uint64 generate();

    /** @returns a new random value between 0 and 1. */
    [[nodiscard]] double generateNormalised();

private:
    //==============================================================================
    Algorithm algorithm = Algorithm::standard;
    const NormalisableRange<double> normaliser { 0.0, (double) std::numeric_limits<uint64>::max() - 1 };

    struct Pimpl;
    struct PimplDeleter final { void operator() (Pimpl*); };
    std::unique_ptr<Pimpl, PimplDeleter> pimpl;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Xorshift64)
};
