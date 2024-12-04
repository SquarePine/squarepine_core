/** Twofish encryption class.

    @see https://github.com/rageworx/libtwofish/blob/main/src/twofish.h
*/
class Twofish final
{
public:
    //==============================================================================
    /** Creates an object that can encode/decode based on the specified key.

       The data must be at least 72 bytes and evenly divisible by 8.
    */
    Twofish (const void* keyData, int keyBytes);

    /** Creates a copy of another Twofish object. */
    Twofish (const Twofish&);

    /** Copies another Twofish object. */
    Twofish& operator= (const Twofish&) noexcept;

    /** Destructor. */
    ~Twofish() noexcept;

    //==============================================================================
    /** Encrypts a pair of 32-bit integers. */
    void encrypt (uint32& data1, uint32& data2) const noexcept;

    /** Decrypts a pair of 32-bit integers. */
    void decrypt (uint32& data1, uint32& data2) const noexcept;

    //==============================================================================
    /** Encrypts a memory block */
    void encrypt (MemoryBlock& data) const;

    /** Decrypts a memory block */
    void decrypt (MemoryBlock& data) const;

    //==============================================================================
    /** Encrypts data in-place

        @param buffer       The message that should be encrypted. See bufferSize on size
                            requirements!
        @param sizeOfMsg    The size of the message that should be encrypted in bytes
        @param bufferSize   The size of the buffer in bytes. To accommodate the encrypted
                            data, the buffer must be larger than the message: the size of
                            the buffer needs to be equal or greater than the size of the
                            message in bytes rounded to the next integer which is divisible
                            by eight. If the message size in bytes is already divisible by eight
                            then you need to add eight bytes to the buffer size. If in doubt
                            simply use bufferSize = sizeOfMsg + 8.

        @returns            The size of the decrypted data in bytes or -1 if the decryption failed.
     */
    int encrypt (void* buffer, size_t sizeOfMsg, size_t bufferSize) const noexcept;

    /** Decrypts data in-place

        @param buffer  The encrypted data that should be decrypted
        @param bytes   The size of the encrypted data in bytes

        @returns       The size of the decrypted data in bytes or -1 if the decryption failed.
    */
    int decrypt (void* buffer, size_t bytes) const noexcept;

private:
    //==============================================================================
    uint32 p[18];
    HeapBlock<uint32> s[4];

    //==============================================================================
    static int pad (void*, size_t, size_t) noexcept;
    static int unpad (const void*, size_t) noexcept;

    bool apply (void*, size_t, void (Twofish::*op) (uint32&, uint32&) const) const;
    uint32 F (uint32) const noexcept;

    //==============================================================================
    JUCE_LEAK_DETECTOR (Twofish)
};
