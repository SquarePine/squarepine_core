/** A stateful and somewhat secure pseudo-random number
    generator encompassing the Blum-Blum-Shub algorithm.

    This uses some BigInteger and seeding tricks to
    create a fairly different BBS instance every construction
    when not specifying a custom `p` and `q`.

    @see https://en.wikipedia.org/wiki/Blum_Blum_Shub
*/
class BlumBlumShub final
{
public:
    /** Constructor. */
    BlumBlumShub();

    /** Constructor where you can specify some initial seeds.

        Only plant seeds manually if you know what you're doing!
        Note that these will get blasted away if the internal RNG
        value hits 0 for whatever reason; the value will be reseeded
        at random.
    */
    BlumBlumShub (uint64 customP, uint64 customQ);

    //==============================================================================
    /** @returns a new random value, between 1 all the way to ULLONG_MAX, in theory. */
    uint64 generate() noexcept;

    /** @returns a new random value, where 0 < value <= ULLONG_MAX,
        albeit normalised between 0 < value <= 1.0.
    */
    double generateNormalised() noexcept;

    //==============================================================================
    /** Not really for public use. */
    using BigM = std::bitset<64>;

private:
    //==============================================================================
    uint64 p = 11, q = 19, m = p * q, x = 4;
    BigM bigM;

    //==============================================================================
    void reseed (uint64 customP, uint64 customQ);
    void reseed();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlumBlumShub)
};
