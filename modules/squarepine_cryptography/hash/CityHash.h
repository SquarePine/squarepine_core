/** CityHash provides hash functions for strings.

    The functions mix the input bits thoroughly but are not suitable for cryptography.

    - hash32() returns a 32-bit hash.
    - hash64() and similar return a 64-bit hash.
    - hash128() and similar return a 128-bit hash and are tuned for
      strings of at least a few hundred bytes.
        - Depending on your compiler and hardware, it's likely faster
          than hash64() on sufficiently long strings.
          It's slower than necessary on shorter strings, but we expect
          that case to be relatively unimportant.

    All members of the CityHash family were designed with heavy reliance
    on previous work by Austin Appleby, Bob Jenkins, and others.
    For example, CityHash32 has many similarities with Murmur3a.

    @see https://github.com/google/cityhash/
    @see https://opensource.googleblog.com/2011/04/introducing-cityhash.html
*/
class CityHash final
{
public:
    //==============================================================================
    /** Hash function for a byte array. */
    static uint32 hash32 (const char* buffer, size_t length);

    /** Hash function for a byte array. */
    static uint64 hash64 (const char* buffer, size_t length);

    /** Hash function for a byte array.
        For convenience, a 64-bit seed is also hashed into the result.
    */
    static uint64 hash64 (const char* buffer, size_t length, uint64 seed);

    /** Hash function for a byte array.
        For convenience, two seeds are also hashed into the result.
    */
    static uint64 hash64 (const char* buffer, size_t length, uint64 seed0, uint64 seed1);

    //==============================================================================
    /** */
    using uint128 = std::pair<uint64, uint64>;

    /** Hash function for a byte array. */
    static uint128 hash128 (const char* buffer, size_t length);

    /** Hash function for a byte array.
        For convenience, a 128-bit seed is also hashed into the result.
    */
    static uint128 hash128 (const char* buffer, size_t length, uint128 seed);

private:
    //==============================================================================
    static constexpr uint32 c1 = 0xcc9e2d51;
    static constexpr uint32 c2 = 0x1b873593;
    static constexpr uint64 k0 = 0xc3a5c85c97cb3127ULL;
    static constexpr uint64 k1 = 0xb492b66fbe98f273ULL;
    static constexpr uint64 k2 = 0x9ae16a3b2f90404fULL;

    static uint64 Fetch64(const char* p) noexcept
    {
        return ByteOrder::swapIfBigEndian(readUnaligned<uint64>(p));
    }

    static uint32 Fetch32(const char* p) noexcept
    {
        return ByteOrder::swapIfBigEndian(readUnaligned<uint32>(p));
    }

    static constexpr uint32 fmix(uint32 h) noexcept
    {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }

    static constexpr uint32 Rotate32(uint32 val, int shift) noexcept
    {
        return shift == 0 ? val : ((val >> shift) | (val << (32 - shift)));
    }

    static constexpr uint64 downhash (uint64 lowBits, uint64 highBits) noexcept
    {
        constexpr uint64 kMul = 0x9ddfea08eb382d69ULL;
        auto a = (lowBits ^ highBits) * kMul;
        a ^= (a >> 47);
        auto b = (highBits ^ a) * kMul;
        b ^= (b >> 47);
        b *= kMul;
        return b;
    }

    static constexpr uint64 Rotate(uint64 val, int shift) noexcept
    {
        return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
    }

    static constexpr uint64 ShiftMix(uint64 val) noexcept
    {
        return val ^ (val >> 47);
    }

    static constexpr uint64 HashLen16(uint64 u, uint64 v, uint64 mul) noexcept
    {
        auto a = (u ^ v) * mul;
        a ^= (a >> 47);
        auto b = (v ^ a) * mul;
        b ^= (b >> 47);
        b *= mul;
        return b;
    }

    static constexpr uint64 HashLen16(uint64 u, uint64 v) noexcept
    {
        return downhash(u, v);
    }

    static uint32 Mur(uint32 a, uint32 h);
    static uint32 Hash32Len0to4(const char* s, size_t len);
    static uint32 Hash32Len5to12(const char* s, size_t len);
    static uint32 Hash32Len13to24(const char* s, size_t len);
    static uint64 HashLen0to16(const char* s, size_t len);
    static uint64 HashLen17to32(const char* s, size_t len);
    static uint64 HashLen33to64(const char* s, size_t len);
    static uint64 CityHash64(const char* s, size_t len);
    static uint128 CityMurmur(const char* s, size_t len, uint128 seed);
    static uint128 WeakHashLen32WithSeeds(uint64 w, uint64 x, uint64 y, uint64 z, uint64 a, uint64 b);
    static uint128 WeakHashLen32WithSeeds(const char* s, uint64 a, uint64 b);

    //==============================================================================
    SQUAREPINE_DECLARE_TOOL_CLASS (CityHash)
};
