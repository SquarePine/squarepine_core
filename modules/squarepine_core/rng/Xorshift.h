/** A stateful, pseudo-random number generator encompassing the Xorshift algorithm.

    Based on multiple sources, the Xorshift algorithm is a modern
    alternative to PSRGs like the Mersenne-Twister, having better
    randomness and improved speed.

    This is a stateful PSRG, therefore keeping one seeded instance around
    to continuously generate a random number is suggested.

    @warning Note that this algorithm, like the Mersenne-Twiser,
             is not cryptographically secure.

    @see https://en.wikipedia.org/wiki/Xorshift
    @see https://www.jstatsoft.org/article/view/v008i14/xorshift.pdf
    @see http://saluc.engr.uconn.edu/refs/crypto/rng/panneton05onthexorshift.pdf
*/
class Xorshift final
{
public:
    /** Constructor that preseeds the system with the current time. */
    Xorshift() noexcept;

    /** Constructor that takes a custom seed. */
    Xorshift (uint32 seed) noexcept;

    //==============================================================================
    /** Generates a new random 32-bit unsigned integral. */
    uint32 generate() noexcept;

private:
    //==============================================================================
    Vector4D<uint32> components;

    //==============================================================================
    void seed (uint32 seed) noexcept;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Xorshift)
};
