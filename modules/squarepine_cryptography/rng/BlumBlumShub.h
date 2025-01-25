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
    /** Constructor */
    BlumBlumShub()
    {
        reseed();
    }

    /** Constructor where you can specify some initial seeds.

        Only plant seeds manually if you know what you're doing!
    */
    BlumBlumShub (uint64 customP, uint64 customQ)
    {
        reseed (customP, customQ);
    }

    //==============================================================================
    /** @returns a new random value. */
    uint64 generate() noexcept
    {
        const auto biggy = square (createBigIntegerFromUint64 (x)) % bigM;

        // uint64 overflow:
        if (biggy.getHighestBit() >= 64)
        {
            /** If you hit this, the `m` was simply too large,
                or you ran this for way too long.
                You'll need to tune your `p` and `q` values to
                fit within the uint64 bit range.
            */
            jassertfalse;
            reseed();
        }
        else
        {
            x = static_cast<uint64> (biggy.toInt64());
        }

        return x;
    }

private:
    //==============================================================================
    uint64 p = 0, q = 0, m = 0, x = 0;
    BigInteger bigM;

    //==============================================================================
    void reseed (uint64 customP = toUint64 (Primes::createProbablePrime (16, 24)),
                 uint64 customQ = toUint64 (Primes::createProbablePrime (16, 26)))
    {
        p = customP;
        q = customQ;
        m = p * q;
        bigM = createBigIntegerFromUint64 (m);
        x = preseed();

        while (! areCoprimes (x, m))
            ++x;
    }

    //==============================================================================
    static uint64 preseed()
    {
        std::random_device device;
        std::mt19937 engine (device());
        std::uniform_int_distribution<uint32> dist (1, std::numeric_limits<uint32>::max() - (uint32) 1);
        return static_cast<uint64> (dist (engine));
    }

    static BigInteger createBigIntegerFromUint64 (uint64 value)
    {
        BigInteger biggy;

        for (uint64 i = 0; i < static_cast<uint64> (64); ++i)
        {
            constexpr auto zero = (uint64) 0;
            constexpr auto one = (uint64) 1;

            biggy.setBit ((int) i, (value & (one << i)) != zero);
        }

        return biggy;
    }

    static uint64 toUint64 (const BigInteger& biggy)
    {
        uint64 r = 0;

        for (int i = biggy.getHighestBit(); --i >= 0;)
        {
            constexpr auto zero = (uint64) 0;
            constexpr auto one = (uint64) 1;

            r |= static_cast<uint64> (one << i) | (biggy[i] ? one : zero);
        }

        return r;
    }

    static uint64 gcd (uint64 a, uint64 b)
    {
        while (b != 0)
        {
            const auto t = b;
            b = a % b;
            a = t;
        }

        return a;
    }

    static bool areCoprimes (uint64 a, uint64 b)
    {
        if (isEven (a) && isEven (b))
            return false;

        return gcd (a, b) == 1;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlumBlumShub)
};
