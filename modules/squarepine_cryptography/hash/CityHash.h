/**

    @see https://github.com/google/cityhash/
*/
class CityHash final
{
public:
    //==============================================================================
    /** Hash function for a byte array. */
    static uint32 hash32 (const char* buffer, size_t length);

    /** Hash function for a byte array. */
    static uint64 hash64 (const char* buffer, size_t length);

    /** Hash function for a byte array. For convenience, a 64-bit seed is also hashed into the result. */
    static uint64 hash64 (const char* buffer, size_t length, uint64 seed);

    /** Hash function for a byte array. For convenience, two seeds are also hashed into the result. */
    static uint64 hash64 (const char* buffer, size_t length, uint64 seed0, uint64 seed1);

    //==============================================================================
    /** */
    using uint128 = std::pair<uint64, uint64>;

    /** */
    constexpr uint64 getLowBits (const uint128& v) noexcept { return v.first; }
    /** */
    constexpr uint64 getHighBits (const uint128& v) noexcept { return v.second; }

    /** Hash function for a byte array. */
    static uint128 hash128 (const char* buffer, size_t length);

    /** Hash function for a byte array. For convenience, a 128-bit seed is also hashed into the result. */
    static uint128 hash128 (const char* buffer, size_t length, uint128 seed);


private:
    //==============================================================================
    // Some primes between 2^63 and 2^64 for various uses.
    static constexpr uint64 k0 = 0xc3a5c85c97cb3127ULL;
    static constexpr uint64 k1 = 0xb492b66fbe98f273ULL;
    static constexpr uint64 k2 = 0x9ae16a3b2f90404fULL;

    // Magic numbers for 32-bit hashing.  Copied from Murmur3.
    static constexpr uint32 c1 = 0xcc9e2d51;
    static constexpr uint32 c2 = 0x1b873593;

    // A 32-bit to 32-bit integer hash copied from Murmur3.
    static uint32 fmix(uint32 h)
    {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }

    static uint32 Rotate32(uint32 val, int shift)
    {
        // Avoid shifting by 32: doing so yields an undefined result.
        return shift == 0 ? val : ((val >> shift) | (val << (32 - shift)));
    }

    /** Hash 128 input bits down to 64 bits of output.
        This is intended to be a reasonably good hash function.
    */
    constexpr uint64 downhash (uint64 lowBits, uint64 highBits) noexcept
    {
        // Murmur-inspired hashing.
        constexpr uint64 kMul = 0x9ddfea08eb382d69ULL;
        auto a = (lowBits ^ highBits) * kMul;
        a ^= (a >> 47);
        auto b = (highBits ^ a) * kMul;
        b ^= (b >> 47);
        b *= kMul;
        return b;
    }

    // Bitwise right rotate. Normally this will compile to a single
    // instruction, especially if the shift is a manifest constant.
    static uint64 Rotate(uint64 val, int shift)
    {
        // Avoid shifting by 64: doing so yields an undefined result.
        return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
    }

    static uint64 ShiftMix(uint64 val)
    {
        return val ^ (val >> 47);
    }

    static uint64 HashLen16(uint64 u, uint64 v, uint64 mul)
    {
        // Murmur-inspired hashing.
        uint64 a = (u ^ v) * mul;
        a ^= (a >> 47);
        uint64 b = (v ^ a) * mul;
        b ^= (b >> 47);
        b *= mul;
        return b;
    }

    static uint64 HashLen16(uint64 u, uint64 v)
    {
        return downhash(u, v);
    }

    //==============================================================================
    SQUAREPINE_DECLARE_TOOL_CLASS (CityHash)
};
