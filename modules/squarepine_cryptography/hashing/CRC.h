/** 

*/
template<typename IntegralType>
struct CRC final
{
    //==============================================================================
    /** The underlying unsigned integral type. */
    using Type                              = std::make_unsigned_t<std::remove_cvref_t<std::remove_pointer_t<IntegralType>>>;
    /** The total number of bits needed for this CRC. */
    static inline constexpr auto numBits    = static_cast<Type> (std::numeric_limits<Type>::digits);
    /** An std::bitset that houses the proper amount of bits to support the CRC value. */
    using BitSet                            = std::bitset<numBits>;

    //==============================================================================
    /** Constructor.

        @param polynomial       Generator polynomial value to use.
        @param initialValue     The value used to initialise the CRC value.
                                If you call reset(), the CRC will be set to this value.
        @param xorOutValue      The final XOR value is XORed to the final CRC value.
                                This is done after the 'Result reflected' step.
        @param shouldReflectIn  If this value is true, each input byte is reflected before being used in the CRC calculation.
                                Reflected means that the bits of the input byte are used in reverse order.
        @param shouldReflectOut If this value is true, the final CRC value is reflected before being returned.
                                The reflection is done over the entirety of CRC value's bits.
    */
    CRC (Type polynomial, Type initialValue, Type xorOutValue,
         bool shouldReflectIn, bool shouldReflectOut) noexcept;

    //==============================================================================
    /** Resets the CRC to the initial, aka XOR-In, value. */
    CRC& reset() noexcept;

    /** @returns the CRC calculated thus far.

        If you're done streaming in data, you should call finalise().
        This will give you the concluded CRC value.
    */
    [[nodiscard]] constexpr Type get() const noexcept   { return crc; }

    /** @returns an appropriately sized std::bitset containing
        the currently calculated CRC.
    */
    [[nodiscard]] BitSet toBitSet() const noexcept      { return crc; }

    //==============================================================================
    /** Processes a single byte into the CRC.
        This is where the bulk of the work happens.
    */
    CRC& processByte (uint8 data) noexcept;

    /** Processes an arbitrary pointer to data. */
    CRC& process (const uint8* data, size_t numBytes) noexcept;

    /** Processes a MemoryBlock. */
    CRC& process (const MemoryBlock& data) noexcept;

    /** Processes an entire File. */
    CRC& process (const File& file);

    /** Processes a String. */
    CRC& processString (const String& data);

    /** You must call this before getting the actual CRC value.

        This is a separate step deliberately because of the nature of calculating CRC values:
        it's not possible to know the final CRC value until all the data has been processed.
    */
    CRC& finalise() noexcept;

    //==============================================================================
    /** @returns an hexadecimal representation of the CRC value.
        This will look something like "0x1234abcd".
    */
    [[nodiscard]] String toHexString() const;

    /** @returns a binary representation of the CRC value.
        This will look something like "0b00000001".
    */
    [[nodiscard]] String toBinString() const;

    /** @returns an octal representation of the CRC value.
        This will look something like "0o00000001".
    */
    [[nodiscard]] String toOctString() const;

    //==============================================================================
    /** @returns the standard check, or test, string.
        This is used to help quickly assert that things are in order.
    */
    static String getCheckString()                      { return "123456789"; }

    /** @returns the CRC value as expected by the check string. */
    Type getCheckValue() const;

private:
    //==============================================================================
    const Type poly, xorIn, xorOut;
    const bool reflectIn, reflectOut;
    std::array<Type, 256> table;
    Type crc;

    static inline constexpr auto numBitsToShift = static_cast<Type> (numBits - 8);

    //==============================================================================
    void populateLookupTable();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRC)
};
