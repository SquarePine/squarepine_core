//==============================================================================
/** */
template<typename Type>
inline constexpr Type cryptographicMaj (Type x, Type y, Type z) noexcept
{
    return (x & y) ^ (x & z) ^ (y & z);
}

/** */
template<typename Type>
inline constexpr Type cryptographicCh (Type x, Type y, Type z) noexcept
{
    return (x & y) ^ (~x & z);
}

//==============================================================================
/** */
template<typename Type>
struct SHA2State final
{
    /** */
    SHA2State() noexcept = default;
    /** */
    constexpr SHA2State (Type a_, Type b_, Type c_, Type d_, Type e_, Type f_, Type g_, Type h_) noexcept :
        a (a_), b (b_), c (c_), d (d_), e (e_), f (f_), g (g_), h (h_) { }

    /** */
    Type getWord (size_t index) const noexcept
    {
        switch (index)
        {
            case 0: return a;
            case 1: return b;
            case 2: return c;
            case 3: return d;
            case 4: return e;
            case 5: return f;
            case 6: return g;
            case 7: return h;
            default: break;
        }

        jassertfalse;
        return 0;
    }

    /** */
    int8 getByte (size_t index) const noexcept
    {
        constexpr auto sizeOfType = sizeof (Type);

        jassert (index < 8 * sizeOfType);

        const auto numWords = index / sizeOfType;
        const auto numBytes = index % sizeOfType;

        return static_cast<int8> (getWord (numWords) >> (sizeOfType - 1 - numBytes) * 8);
    }

    /** */
    String toString (size_t numBytes) const noexcept
    {
        String bytes;
        bytes.preallocateBytes (numBytes);

        for (size_t i = 0; i < numBytes; ++i)
            bytes << String::charToString (getByte (i));

        return bytes;
    }

    /** */
    SHA2State& operator+= (const SHA2State& rhs) noexcept
    {
        a += rhs.a;
        b += rhs.b;
        c += rhs.c;
        d += rhs.d;
        e += rhs.e;
        f += rhs.f;
        g += rhs.g;
        h += rhs.h;

        return *this;
    }

    Type a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0, h = 0;
};

//==============================================================================
/** */
template<typename Type>
struct SHA2Block final
{
    //==============================================================================
    /** */
    static constexpr size_t blockSize = 16u;
    /** */
    using ArrayType = typename std::array<Type, blockSize>;

    //==============================================================================
    /** */
    SHA2Block() noexcept = default;

    /** */
    constexpr SHA2Block (const Type* ptr) noexcept
    {
        for (size_t i = 0; i < blockSize; ++i)
            setByte (i, *ptr++);
    }

    //==============================================================================
    /** */
    void setByte (size_t index, Type value) noexcept
    {
        data[index] |= value;
    }

    //==============================================================================
    /** */
    constexpr const Type& operator[] (size_t i) const noexcept { return data[i % blockSize]; }
    /** */
    constexpr Type& operator[] (size_t i) noexcept { return data[i % blockSize]; }

    //==============================================================================
    ArrayType data;
};

//==============================================================================
template<typename Type, size_t numBits>
class SHA2
{
public:
    //==============================================================================
    /** */
    using State = SHA2State<Type>;
    /** */
    using Block = SHA2Block<Type>;
    /** */
    using ArrayType = typename Block::ArrayType;

    //==============================================================================
    /** */
    SHA2 (Type a, Type b, Type c, Type d, Type e, Type f, Type g, Type h) noexcept :
        state (a, b, c, d, e, f, g, h),
        overflowIter (overflow.begin())
    {
        static_assert (numBits % 8 == 0);
    }

    /** */
    virtual ~SHA2() noexcept = default;

    //==============================================================================
    /** */
    SHA2& add (const int8* ptr, const int8* last, bool finish = true) noexcept
    {
        size += last - ptr;

        if (overflowIter != overflow.begin())
        {
            addToOverflow (ptr, last);

            if (overflowIter == overflow.end())
            {
                add (Block (overflow.data()));
                overflowIter = overflow.begin();

            }
            else
            {
                if (finish)
                    pad();

                return *this;
            }
        }

        while (ptr + Block::blockSize <= last)
        {
            add (Block (ptr));
            ptr += Block::blockSize;
        }

        addToOverflow (ptr, last);

        if (finish)
            pad();

        return *this;
    }

    /** */
    SHA2& add (const String& str, bool finish = true) noexcept
    {
        auto utf8Data = str.toUTF8();

        const auto first = (const int8*) utf8Data.getAddress();
        const auto last = first + utf8Data.sizeInBytes();

        return add (first, last, finish);
    }

    //==============================================================================
    /** */
    String toString() const noexcept { return state.toString (numBits / 8); }

private:
    //==============================================================================
    static constexpr size_t numRounds = sizeof (Type) == 4 ? 64 : 80;
    static constexpr size_t padLengthOfLength = sizeof (Type) == 4 ? 8 : 16;

    State state;
    size_t size = 0;

    ArrayType overflow;
    ArrayType::iterator overflowIter = {};

    //==============================================================================
    /** */
    static uint32 K32 (size_t i) noexcept
    {
        constexpr uint32 K32[64] =
        {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };

        jassert (i < (size_t) (int) std::size (K32));
        return static_cast<Type> (K32[i]);
    }

    /** */
    static uint64 K64 (size_t i) noexcept
    {
        constexpr uint64 K64[80] =
        {
            0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538,
            0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe,
            0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
            0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
            0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab,
            0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
            0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed,
            0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
            0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
            0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
            0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373,
            0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
            0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c,
            0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6,
            0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
            0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
        };

        jassert (i < (size_t) (int) std::size (K64));
        return static_cast<Type> (K64[i]);
    }

    //==============================================================================
    /** */
    template<int A, int B, int C>
    static uint32 S (uint32 x) noexcept     { return rotr (x, A) ^ rotr (x, B) ^ rotr (x, C); }
    /** */
    template<int A, int B, int C>
    static uint64 S (uint64 x) noexcept     { return rotr (x, A) ^ rotr (x, B) ^ rotr (x, C); }

    /** */
    template<int A, int B, int C>
    static uint32 s (uint32 x) noexcept     { return rotr (x, A) ^ rotr (x, B) ^ x >> C; }
    /** */
    template<int A, int B, int C>
    static uint64 s (uint64 x) noexcept     { return rotr (x, A) ^ rotr (x, B) ^ x >> C; }

    /** */
    static uint32 S0 (uint32 x) noexcept    { return S<2, 13, 22> (x);  }
    /** */
    static uint64 S0 (uint64 x) noexcept    { return S<28, 34, 39> (x); }

    /** */
    static uint32 S1 (uint32 x) noexcept    { return S<6, 11, 25> (x); }
    /** */
    static uint64 S1 (uint64 x) noexcept    { return S<14, 18, 41> (x); }

    /** */
    static uint32 s0 (uint32 x) noexcept    { return s<7, 18, 3> (x); }
    /** */
    static uint64 s0 (uint64 x) noexcept    { return s<1, 8, 7> (x); }

    /** */
    static uint32 s1 (uint32 x) noexcept    { return s<17, 19, 10> (x); }
    /** */
    static uint64 s1 (uint64 x) noexcept    { return s<19, 61, 6> (x); }

    //==============================================================================
    /** */
    static State round (const State& tmp, Type K, Type W) noexcept
    {
        const auto T1 = tmp.h + S1 (tmp.e) + cryptographicCh (tmp.e, tmp.f, tmp.g) + K + W;
        const auto T2 = S0 (tmp.a) + cryptographicMaj (tmp.a, tmp.b, tmp.c);

        return
        {
            T1 + T2,
            tmp.a,
            tmp.b,
            tmp.c,
            tmp.d + T1,
            tmp.e,
            tmp.f,
            tmp.g
        };
    }

    //==============================================================================
    /** */
    void add (Block W) noexcept
    {
        auto tmp = state;

        if (std::is_same<Type, uint32>::value)
        {
            for (size_t i = 0; i < Block::blockSize; ++i)
                tmp = round (tmp, K32 (i), W[i]);

            for (size_t i = Block::blockSize; i < numRounds; ++i)
            {
                const auto W_ = s1 (W[i - 2])
                              + W[i - 7]
                              + s0 (W[i - 15])
                              + W[i - 16];

                tmp = round (tmp, K32 (i), W_);

                W[i] = W_;
            }
        }
        else
        {
            for (size_t i = 0; i < Block::blockSize; ++i)
                tmp = round (tmp, K64 (i), W[i]);

            for (size_t i = Block::blockSize; i < numRounds; ++i)
            {
                const auto W_ = s1 (W[i - 2])
                              + W[i - 7]
                              + s0 (W[i - 15])
                              + W[i - 16];

                tmp = round (tmp, K64 (i), W_);

                W[i] = W_;
            }
        }

        state += tmp;
    }

    /** */
    void addToOverflow (const int8*& ptr, const int8* last) noexcept
    {
        while (overflowIter != overflow.end() && ptr != last)
            *overflowIter++ = *ptr++;
    }

    /** */
    void pad() noexcept
    {
        jassert (overflowIter != overflow.end());

        // Add the terminating '1' bit.
        *overflowIter++ = 0x80;

        // Complete the current SHA2Block if there is not enough room
        // for the length in this SHA2Block.
        const auto overflowLeft = overflow.end() - overflowIter;

        if (overflowLeft < padLengthOfLength)
        {
            while (overflowIter != overflow.end())
                *overflowIter++ = 0;

            add (Block (overflow.data()));
            overflowIter = overflow.begin();
        }

        // Pad until the start of length.
        const auto overflowLengthStart = overflow.end() - padLengthOfLength;
        while (overflowIter != overflowLengthStart)
            *overflowIter++ = 0;

        const auto numBitsLocal = size * 8;

        for (int64 i = padLengthOfLength; --i >= 0;)
            *overflowIter++ = i < (int64) sizeof (Type)
                                ? static_cast<int8> (numBitsLocal >> i * 8)
                                : 0;

        add (Block (overflow.data()));
    }
};

//==============================================================================
struct SHA224 final : SHA2<uint32, 224>
{
    SHA224() noexcept :
        SHA2<uint32, 224> (0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
                           0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4)
    {
    }
};

struct SHA256 final : SHA2<uint32, 256>
{
    SHA256() noexcept :
        SHA2<uint32, 256> (0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                           0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19)
    {
    }
};

struct SHA384 final : SHA2<uint64, 384>
{
    SHA384() noexcept :
        SHA2<uint64, 384> (0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939,
                           0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4)
    {
    }
};

struct SHA512 final : SHA2<uint64, 512>
{
    SHA512() noexcept :
        SHA2<uint64, 512> (0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
                           0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179)
    {
    }
};

struct SHA512_224 final : SHA2<uint64, 224>
{
    SHA512_224() noexcept :
        SHA2<uint64, 224> (0x8C3D37C819544DA2, 0x73E1996689DCD4D6, 0x1DFAB7AE32FF9C82, 0x679DD514582F9FCF,
                           0x0F6D2B697BD44DA8, 0x77E36F7304C48942, 0x3F9D85A86A1D36C8, 0x1112E6AD91D692A1)
    {
    }
};

struct SHA512_256 final : SHA2<uint64, 256>
{
    SHA512_256() noexcept :
        SHA2<uint64, 256> (0x22312194FC2BF72C, 0x9F555FA3C84C64C2, 0x2393B86B6F53B151, 0x963877195940EABD,
                           0x96283EE2A88EFFE3, 0xBE5E1E2553863992, 0x2B0199FC2C85B8AA, 0x0EB72DDC81C52CA2)
    {
    }
};

#if 0
    SHA224          sha224;
    sp::SHA256     sha256;
    SHA384          sha384;
    SHA512_224      sha512_224;
    SHA512_256      sha512_256;

    sha224.add ("test");
    sha256.add ("test");
    sha384.add ("test");
    sha512_224.add ("test");
    sha512_256.add ("test");

    DBG (sha224.toString());
    DBG (sha256.toString());
    DBG (sha384.toString());
    DBG (sha512_224.toString());
    DBG (sha512_256.toString());
#endif
