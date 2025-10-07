namespace bbs
{
    constexpr std::optional<int> getMSB (const BlumBlumShub::BigM& set)
    {
        for (int i = (int) set.size(); --i >= 0;)
            if (set.test ((size_t) i))
                return { i };

        return std::nullopt;
    }

    inline BlumBlumShub::BigM toBitset (uint64 value) noexcept
    {
        return BlumBlumShub::BigM (static_cast<unsigned long long> (value));
    }

    inline uint64 fromBitset (const BlumBlumShub::BigM& biggy) noexcept
    {
        return static_cast<uint64> (biggy.to_ullong());
    }

    constexpr uint64 gcd (uint64 a, uint64 b)
    {
        while (b != 0)
        {
            const auto t = b;
            b = a % b;
            a = t;
        }

        return a;
    }

    constexpr bool areCoprimes (uint64 a, uint64 b)
    {
        if (isEven (a) && isEven (b))
            return false;

        return gcd (a, b) == 1;
    }

    inline uint64 createProbablePrime()
    {
        const auto biggy = Primes::createProbablePrime (32, 30);
        const auto hsb = biggy.getHighestBit();
        BlumBlumShub::BigM bits;

        for (int i = 0; i < hsb; ++i)
            bits.set ((size_t) i, biggy[i]);

        return fromBitset (bits);
    }
}

//==============================================================================
BlumBlumShub::BlumBlumShub()                                { reseed(); }
BlumBlumShub::BlumBlumShub (uint64 customP, uint64 customQ) { reseed (customP, customQ); }

uint64 BlumBlumShub::generate() noexcept
{
    x = square (x) % bbs::fromBitset (bigM);
    while (x == 0)
        reseed();

    return x;
}

double BlumBlumShub::generateNormalised() noexcept
{
    const auto nextValue = generate();
    return std::lerp ((double) 0.0, (double) (ULLONG_MAX - 1ULL), (double) nextValue);
}

void BlumBlumShub::reseed()
{
    reseed (bbs::createProbablePrime(), bbs::createProbablePrime());
}

void BlumBlumShub::reseed (uint64 customP, uint64 customQ)
{
    // preseed 'x' to something non-static:
    x = []() -> uint64
    {
        std::random_device device;
        std::mt19937_64 engine (device());
        std::uniform_int_distribution<uint64> dist (1, ULLONG_MAX - 1ULL);
        return static_cast<uint64> (dist (engine));
    }();

    p = customP;
    q = customQ;
    m = p * q;
    bigM = bbs::toBitset (m);

    while (! bbs::areCoprimes (x, m))
        ++x;
}
