/** A stateful, pseudo-random number generator encompassing the Xorshift algorithm.

    Based on multiple sources, the Xorshift algorithm is a modern
    alternative to PSRGs like the Mersenne-Twister, having better
    randomness and improved speed.

    This is a stateful PSRG, therefore keeping one seeded instance around
    to continuously generate a random number is suggested. That being said,
    this is in no way thread-safe.

    @warning Note that this algorithm, like the Mersenne-Twiser,
             is not cryptographically secure.

    @see https://en.wikipedia.org/wiki/Xorshift
    @see https://www.jstatsoft.org/article/view/v008i14/xorshift.pdf
    @see http://saluc.engr.uconn.edu/refs/crypto/rng/panneton05onthexorshift.pdf
*/
class Xorshift final
{
public:
    /** Constructor that takes a custom seed. */
    Xorshift (uint32 seedToStartWith) noexcept :
        components (seed (seedToStartWith))
    {
    }

    /** Constructor that preseeds the system with the current time. */
    Xorshift() noexcept :
        Xorshift (static_cast<uint32> (Time::currentTimeMillis()))
    {
    }

    //==============================================================================
    /** Generates a new random 32-bit unsigned integral. */
    uint32 generate() noexcept
    {
        auto t = components.x;
        t ^= t << 11;
        t ^= t >> 8;
        components.x = components.y;
        components.y = components.z;
        components.z = components.w;
        components.w ^= components.w >> 19;
        components.w ^= t;
        return components.w;
    }

private:
    //==============================================================================
    Vector4D<uint32> components;

    //==============================================================================
    static constexpr Vector4D<uint32> seed (uint32 seed) noexcept
    {
        return { seed << 11, seed >> 3, seed << 5, seed >> 2 };
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Xorshift)
};
