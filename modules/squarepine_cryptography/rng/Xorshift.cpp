namespace xorshiftFuncs
{
    constexpr uint64 preseed (uint64& seed) noexcept
    {
        seed += 0x9e3779b97f4a7c15ULL;
        seed = (seed ^ (seed >> 30)) * 0xbf58476d1ce4e5b9ULL;
        seed = (seed ^ (seed >> 27)) * 0x94d049bb133111ebULL;
        return seed ^ (seed >> 31);
    }

    template<typename ValueType>
    constexpr ValueType rol64 (ValueType x, ValueType k) noexcept
    {
        return (x << k) | (x >> (64 - k));
    }

    uint64 xorshift64 (std::array<uint64, 1>& state) noexcept
    {
        auto& value = state[0];
        value ^= value << 13;
        value ^= value >> 7;
        value ^= value << 17;
        return value;
    }

    uint64 xorshift64Star (std::array<uint64, 1>& state) noexcept
    {
        auto& value = state[0];
        value ^= value >> 12;
        value ^= value << 25;
        value ^= value >> 27;
        return value * 0x2545f4914f6cdd1dULL;
    }

    uint64 xorshift128p (std::array<uint64, 2>& state) noexcept
    {
        auto t = state[0];
        const auto s = state[1];
        state[0] = s;
        t ^= t << 23;
        t ^= t >> 18;
        t ^= s ^ (s >> 5);
        state[1] = t;
        return t + s;
    }

    uint64 xoshiro256pp (std::array<uint64, 4>& state) noexcept
    {
        const auto result = rol64 (state[0] + state[3], 23ULL) + state[0];
        const auto t = state[1] << 17;

        state[2] ^= state[0];
        state[3] ^= state[1];
        state[1] ^= state[2];
        state[0] ^= state[3];
        state[2] ^= t;
        state[3] = rol64 (state[3], 45ULL);

        return result;
    }

    uint64 xoshiro256ss (std::array<uint64, 4>& state) noexcept
    {
        const auto result = rol64 (state[1] * 5ULL, 7ULL) * 9;
        const auto t = state[1] << 17;

        state[2] ^= state[0];
        state[3] ^= state[1];
        state[1] ^= state[2];
        state[0] ^= state[3];
        state[2] ^= t;
        state[3] = rol64 (state[3], 45ULL);

        return result;
    }

    uint64 xoshiro256p (std::array<uint64, 4>& state) noexcept
    {
        const auto result = state[0] + state[3];
        const auto t = state[1] << 17;

        state[2] ^= state[0];
        state[3] ^= state[1];
        state[1] ^= state[2];
        state[0] ^= state[3];
        state[2] ^= t;
        state[3] = rol64 (state[3], 45ULL);

        return result;
    }

    uint64 xorshift1024s (std::array<uint64, 16>& state, int& index) noexcept
    {
        auto i = (size_t) index;

        auto s = state[i++];
        i &= 15;
        auto t = state[i];
        t ^= t << 31;
        t ^= t >> 11;
        t ^= s ^ (s >> 30);
        state[i] = t;

        index = (int) i;

        return t * 1181783497276652981ULL;
    }
}

//==============================================================================
#if JUCE_MSVC
 #pragma pack (push, 1)
#endif

class XorshiftForwarder
{
public:
    XorshiftForwarder() = default;
    virtual ~XorshiftForwarder() = default;

    virtual void reset (uint64 seedToStartWith) = 0;
    virtual uint64 generate() = 0;

protected:
    template<size_t numElements>
    static void resetInternal (std::array<uint64, numElements>& data,
                               uint64 seedToStartWith)
    {
        data[0] = seedToStartWith;

        for (auto& v : data)
            v = xorshiftFuncs::preseed (v);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XorshiftForwarder)
} JUCE_PACKED;

template<auto func, size_t numElements>
class Xorshift64Base : public XorshiftForwarder
{
public:
    Xorshift64Base() = default;

    uint64 generate() override                      { return func (data); }
    void reset (uint64 seedToStartWith) override    { resetInternal (data, seedToStartWith); }

private:
    std::array<uint64, numElements> data;
} JUCE_PACKED;

class Xorshift1024s final : public XorshiftForwarder
{
public:
    Xorshift1024s() = default;

    uint64 generate() override                      { return xorshiftFuncs::xorshift1024s (data, index); }
    void reset (uint64 seedToStartWith) override    { resetInternal (data, seedToStartWith); }

private:
    std::array<uint64, 16> data;
    int index = 0;
} JUCE_PACKED;

struct Xorshift64::Pimpl final
{
    using Standard      = Xorshift64Base<xorshiftFuncs::xorshift64, 1>;
    using Star          = Xorshift64Base<xorshiftFuncs::xorshift64Star, 1>;
    using Xorshift128p  = Xorshift64Base<xorshiftFuncs::xorshift128p, 2>;
    using Xoshiro256pp  = Xorshift64Base<xorshiftFuncs::xoshiro256pp, 4>;
    using Xoshiro256ss  = Xorshift64Base<xorshiftFuncs::xoshiro256ss, 4>;
    using Xoshiro256p   = Xorshift64Base<xorshiftFuncs::xoshiro256p, 4>;

    Pimpl (uint64 seedToStartWith) :
        forwarder (std::make_unique<Pimpl::Standard>())
    {
        forwarder->reset (seedToStartWith);
    }

    ~Pimpl() = default;

    std::unique_ptr<XorshiftForwarder> forwarder;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
} JUCE_PACKED;

#if JUCE_MSVC
 #pragma pack (pop)
#endif

void Xorshift64::PimplDeleter::operator() (Xorshift64::Pimpl* pimpl)
{
    deleteAndZero (pimpl);
}

//==============================================================================
Xorshift32::Xorshift32 (uint32 seedToStartWith) :
    state (seedToStartWith)
{
}

Xorshift32::Xorshift32() :
    Xorshift32 (static_cast<uint32> (std::abs (Time::currentTimeMillis())))
{
}

uint32 Xorshift32::generate()           { return xorshift (state); }
float Xorshift32::generateNormalised()  { return normaliser.convertTo0to1 ((float) generate()); }

uint32 Xorshift32::xorshift (uint32& x) noexcept
{
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

//==============================================================================
Xorshift64::Xorshift64 (uint64 seedToStartWith, Algorithm algo) :
    pimpl (new Pimpl (seedToStartWith))
{
    setAlgorithm (algo);
}

Xorshift64::Xorshift64 (Algorithm algo) :
    Xorshift64 (static_cast<uint64> (std::abs (Time::currentTimeMillis())), algo)
{
}

uint64 Xorshift64::generate()           { return pimpl->forwarder->generate(); }
double Xorshift64::generateNormalised() { return normaliser.convertTo0to1 ((double) generate()); }

void Xorshift64::setAlgorithm (Algorithm newAlgorithmToChoose)
{
    if (algorithm == newAlgorithmToChoose)
        return;

    auto preseed = generate();

    algorithm = newAlgorithmToChoose;
    switch (algorithm)
    {
        case Algorithm::standard:       pimpl->forwarder = std::make_unique<Pimpl::Standard>(); break;
        case Algorithm::star:           pimpl->forwarder = std::make_unique<Pimpl::Star>(); break;
        case Algorithm::xorshift128p:   pimpl->forwarder = std::make_unique<Pimpl::Xorshift128p>(); break;
        case Algorithm::xoshiro256pp:   pimpl->forwarder = std::make_unique<Pimpl::Xoshiro256pp>(); break;
        case Algorithm::xoshiro256ss:   pimpl->forwarder = std::make_unique<Pimpl::Xoshiro256ss>(); break;
        case Algorithm::xoshiro256p:    pimpl->forwarder = std::make_unique<Pimpl::Xoshiro256p>(); break;
        case Algorithm::xorshift1024s:  pimpl->forwarder = std::make_unique<Xorshift1024s>(); break;

        default: jassertfalse; break;
    };

    pimpl->forwarder->reset (preseed);
}
