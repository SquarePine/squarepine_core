/** A stateful and cryptographically secure pseudo-random number generator
    using the ISAAC algorithm.

    ISAAC stands for "indirection, shift, accumulate, add, and count."

    Keeping one seeded instance around to continuously
    generate a random number is suggested seeing that it requires
    extra processing to initialise.

    This algorithm is intended to be cryptographically secure.

    All told, ISAAC requires an amortised 18.75 instructions to produce each 32-bit value.
    (With the same optimizations, IA requires an amortised
    12.56 instructions to produce each 32-bit value.
    There are no cycles in ISAAC shorter than 240 values.
    There are no bad initial states.
    The internal state has 8288 bits, so the expected cycle length
    is 28287 calls (or 28295 32-bit values).
    Deducing the internal state appears to be intractable, and the results
    of ISAAC are unbiased and uniformly distributed.

    This is a stateful PSRG, therefore keeping one seeded instance around
    to continuously generate a random number is suggested. That being said,
    this is in no way thread-safe.

    @see https://en.wikipedia.org/wiki/ISAAC_(cipher)
    @see http://burtleburtle.net/bob/rand/isaac.html
    @see http://burtleburtle.net/bob/cplus/isaac.hpp
*/
class ISAAC final
{
public:
    /** */
    ISAAC (bool preseedResults = true);

    //==============================================================================
    /** Generates a random 32-bit unsigned integral. */
    uint32 generate();

    /** @returns a new random value, where 0 < value <= ULONG_MAX,
        albeit normalised between 0 < value <= 1.0.
    */
    double generateNormalised() noexcept;

private:
    //==============================================================================
    enum
    {
        alpha = 8,
        maxNumItems = 256,
        halfNumItems = maxNumItems / 2,

        /** @see http://softwareengineering.stackexchange.com/a/63605 */
        goldenRatio = 0x9e3779b9
    };

    //==============================================================================
    Vector3D<uint32> randomValues;
    uint32 randMemory[maxNumItems];
    uint32 results[maxNumItems];
    uint32 count = 0;

    //==============================================================================
    static constexpr uint32 leftShift (uint32 source, uint32 amount) noexcept   { return source << amount; }
    static constexpr uint32 rightShift (uint32 source, uint32 amount) noexcept  { return source >> amount; }

    template<uint32 shiftAmount>
    static void shuffle (std::function<uint32 (uint32, uint32)> shiftFunction,
                         uint32& a, uint32& b, uint32& c, uint32& d) noexcept
    {
        a ^= shiftFunction (b, shiftAmount);
        d += a;
        b += c;
    }

    static void shuffle (uint32& a, uint32& b, uint32& c, uint32& d,
                         uint32& e, uint32& f, uint32& g, uint32& h) noexcept;

    /** Note that bits 2..9 are chosen from x but 10..17 are chosen from y.

        The only important thing here is that 2..9 and 10..17 don't overlap.

        2..9 and 10..17 were then chosen for speed in the optimised version (randomValues.z)

        See http://burtleburtle.net/bob/rand/isaac.html for further explanations and analysis.
    */
    void next();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ISAAC)
};
