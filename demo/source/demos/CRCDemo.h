//==============================================================================
/** Enumeration for CRC table column identifiers */
enum class CRCColumnID
{
    Bits = 1,
    Poly,
    XorIn,
    XorOut,
    RefIn,
    RefOut,
    CheckValue,
    DecResult,
    HexResult
};

//==============================================================================
/** A data unit that manages CRC calculations for different bit lengths.

    This class provides a unified interface for CRC operations across 8, 16, 32, and 64-bit
    implementations. It automatically creates and manages the appropriate CRC atom based on
    the configured bit count.

    @see CRC8Atom, CRC16Atom, CRC32Atom, CRC64Atom
*/
class CRCDataUnit final
{
public:
    CRCDataUnit() = default;
    ~CRCDataUnit() = default;

    /** Configures the CRC data unit with the specified parameters.

        This method creates and configures the appropriate CRC atom based on the bit count.
        Only one CRC atom can be active at a time.

        @param numBits   The number of bits for the CRC calculation (8, 16, 32, or 64)
        @param poly      The polynomial value for the CRC algorithm
        @param xorIn     The initial XOR value
        @param xorOut    The final XOR value
        @param refIn     Whether to reflect the input bits
        @param refOut    Whether to reflect the output bits
    */
    void configure (int numBits, uint64 poly, uint64 xorIn,
                    uint64 xorOut, bool refIn, bool refOut)
    {
        SQUAREPINE_CRASH_TRACER

        // Store original parameters
        originalPoly = poly;
        originalXorIn = xorIn;
        originalXorOut = xorOut;
        originalRefIn = refIn;
        originalRefOut = refOut;

        // Clear any existing CRC atoms
        crc8.reset();
        crc16.reset();
        crc32.reset();
        crc64.reset();

        // Create and configure the appropriate CRC atom based on bit count
        switch (numBits)
        {
            case 8:
                crc8 = std::make_unique<CRC8Atom>();
                crc8->configure (poly, xorIn, xorOut, refIn, refOut);
            break;

            case 16:
                crc16 = std::make_unique<CRC16Atom>();
                crc16->configure (poly, xorIn, xorOut, refIn, refOut);
            break;

            case 32:
                crc32 = std::make_unique<CRC32Atom>();
                crc32->configure (poly, xorIn, xorOut, refIn, refOut);
            break;

            case 64:
                crc64 = std::make_unique<CRC64Atom>();
                crc64->configure (poly, xorIn, xorOut, refIn, refOut);
            break;

            default:
                // Invalid bit count - leave all atoms as nullptr
                jassertfalse;
            break;
        }
    }

    //==============================================================================
    /** @returns the number of bits for the current CRC configuration, or 0 if not configured */
    uint8 getNumBits() const                    { auto* atom = getActiveAtom(); return atom != nullptr ? atom->getNumBits() : 0; }

    /** @returns the standard check value for the current CRC configuration, or 0 if not configured */
    uint64 getCheckValue() const                { auto* atom = getActiveAtom(); return atom != nullptr ? atom->getCheckValue() : 0; }

    /** @returns the standard check string for the current CRC configuration, or empty string if not configured */
    String getCheckString() const               { auto* atom = getActiveAtom(); return atom != nullptr ? atom->getCheckString() : String{}; }

    /** @returns the final CRC value for the current configuration, or 0 if not configured */
    uint64 getFinalValue() const                { auto* atom = getActiveAtom(); return atom != nullptr ? atom->getFinalValue() : 0; }

    /** @returns the final CRC value as a decimal string, or empty string if not configured */
    String getFinalValueAsDecString() const     { auto* atom = getActiveAtom(); return atom != nullptr ? String (atom->getFinalValue()) : String{}; }

    /** @returns the final CRC value as a hexadecimal string, or empty string if not configured */
    String getFinalValueAsHexString() const     { auto* atom = getActiveAtom(); return atom != nullptr ? atom->getFinalValueAsHexString() : String{}; }

    /** @returns the number of bytes for the current CRC configuration, or 0 if not configured */
    uint8 getNumBytes() const
    {
        if (const auto numBits = getNumBits(); numBits > 0)
            return numBits / 8;

        return 0; // Not configured...
    }

    //==============================================================================
    /** @returns the original polynomial value used for configuration */
    constexpr uint64 getOriginalPoly() const noexcept   { return originalPoly; }

    /** @returns the original xorIn value used for configuration */
    constexpr uint64 getOriginalXorIn() const noexcept  { return originalXorIn; }

    /** @returns the original xorOut value used for configuration */
    constexpr uint64 getOriginalXorOut() const noexcept { return originalXorOut; }

    /** @returns the original refIn value used for configuration */
    constexpr bool getOriginalRefIn() const noexcept    { return originalRefIn; }

    /** @returns the original refOut value used for configuration */
    constexpr bool getOriginalRefOut() const noexcept   { return originalRefOut; }

private:
    //==============================================================================
    /** Base class for CRC atom implementations.

        This abstract class defines the interface that all CRC atom types must implement.
        Each atom type handles a specific bit length and delegates to the underlying CRC class.

        @see CRC8Atom, CRC16Atom, CRC32Atom, CRC64Atom
    */
    struct CRCAtom
    {
        CRCAtom() = default;
        virtual ~CRCAtom() = default;

        /** Configures the CRC atom with the specified parameters.

            @param poly      The polynomial value for the CRC algorithm
            @param xorIn     The initial XOR value
            @param xorOut    The final XOR value
            @param refIn     Whether to reflect the input bits
            @param refOut    Whether to reflect the output bits
        */
        virtual void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) = 0;

        /** @returns the number of bits for this CRC atom */
        virtual uint8 getNumBits() const = 0;

        /** @returns the standard check value for this CRC configuration */
        virtual uint64 getCheckValue() const = 0;

        /** @returns the standard check string for this CRC configuration */
        virtual String getCheckString() const = 0;

        /** @returns the final CRC value for this configuration */
        virtual uint64 getFinalValue() const = 0;

        /** @returns the final CRC value as a decimal string */
        virtual String getFinalValueAsDecString() const = 0;

        /** @returns the final CRC value as a hexadecimal string */
        virtual String getFinalValueAsHexString() const = 0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCAtom)
    };

    /** 8-bit CRC atom implementation.

        This class handles 8-bit CRC calculations by delegating to the underlying CRC<uint8> class.
    */
    struct CRC8Atom final : public CRCAtom
    {
        CRC8Atom() = default;

        void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) override
        {
            crc = std::make_unique<CRCType> ((Type) poly, (Type) xorIn, (Type) xorOut, refIn, refOut);
        }

        uint8 getNumBits() const override                   { return 8; }
        uint64 getCheckValue() const override               { return crc != nullptr ? crc->getCheckValue() : 0; }
        String getCheckString() const override              { return crc != nullptr ? crc->getCheckString() : String{}; }
        uint64 getFinalValue() const override               { return crc != nullptr ? crc->get() : 0; }
        String getFinalValueAsDecString() const override    { return crc != nullptr ? String (crc->get()) : String{}; }
        String getFinalValueAsHexString() const override    { return crc != nullptr ? crc->toHexString() : String{}; }

        using Type = uint8;
        using CRCType = CRC<Type>;
        std::unique_ptr<CRCType> crc;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRC8Atom)
    };

    /** 16-bit CRC atom implementation.

        This class handles 16-bit CRC calculations by delegating to the underlying CRC<uint16> class.
    */
    struct CRC16Atom final : public CRCAtom
    {
        CRC16Atom() = default;

        void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) override
        {
            crc = std::make_unique<CRCType> ((Type) poly, (Type) xorIn, (Type) xorOut, refIn, refOut);
        }

        uint8 getNumBits() const override                   { return 16; }
        uint64 getCheckValue() const override               { return crc != nullptr ? crc->getCheckValue() : 0; }
        String getCheckString() const override              { return crc != nullptr ? crc->getCheckString() : String{}; }
        uint64 getFinalValue() const override               { return crc != nullptr ? crc->get() : 0; }
        String getFinalValueAsDecString() const override    { return crc != nullptr ? String (crc->get()) : String{}; }
        String getFinalValueAsHexString() const override    { return crc != nullptr ? crc->toHexString() : String{}; }

        using Type = uint16;
        using CRCType = CRC<Type>;
        std::unique_ptr<CRCType> crc;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRC16Atom)
    };

    /** 32-bit CRC atom implementation.

        This class handles 32-bit CRC calculations by delegating to the underlying CRC<uint32> class.
    */
    struct CRC32Atom final : public CRCAtom
    {
        CRC32Atom() = default;

        void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) override
        {
            crc = std::make_unique<CRCType> ((Type) poly, (Type) xorIn, (Type) xorOut, refIn, refOut);
        }

        uint8 getNumBits() const override                   { return 32; }
        uint64 getCheckValue() const override               { return crc != nullptr ? crc->getCheckValue() : 0; }
        String getCheckString() const override              { return crc != nullptr ? crc->getCheckString() : String{}; }
        uint64 getFinalValue() const override               { return crc != nullptr ? crc->get() : 0; }
        String getFinalValueAsDecString() const override    { return crc != nullptr ? String (crc->get()) : String{}; }
        String getFinalValueAsHexString() const override    { return crc != nullptr ? crc->toHexString() : String{}; }

        using Type = uint32;
        using CRCType = CRC<Type>;
        std::unique_ptr<CRCType> crc;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRC32Atom)
    };

    /** 64-bit CRC atom implementation.

        This class handles 64-bit CRC calculations by delegating to the underlying CRC<uint64> class.
    */
    struct CRC64Atom final : public CRCAtom
    {
        CRC64Atom() = default;

        void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) override
        {
            crc = std::make_unique<CRCType> ((Type) poly, (Type) xorIn, (Type) xorOut, refIn, refOut);
        }

        uint8 getNumBits() const override                   { return 64; }
        uint64 getCheckValue() const override               { return crc != nullptr ? crc->getCheckValue() : 0; }
        String getCheckString() const override              { return crc != nullptr ? crc->getCheckString() : String{}; }
        uint64 getFinalValue() const override               { return crc != nullptr ? crc->get() : 0; }
        String getFinalValueAsDecString() const override    { return crc != nullptr ? String (crc->get()) : String{}; }
        String getFinalValueAsHexString() const override    { return crc != nullptr ? crc->toHexString() : String{}; }

        using Type = uint64;
        using CRCType = CRC<Type>;
        std::unique_ptr<CRCType> crc;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRC64Atom)
    };

    std::unique_ptr<CRC8Atom> crc8;
    std::unique_ptr<CRC16Atom> crc16;
    std::unique_ptr<CRC32Atom> crc32;
    std::unique_ptr<CRC64Atom> crc64;

    // Store original configuration parameters
    uint64 originalPoly = 0,
           originalXorIn = 0,
           originalXorOut = 0;
    bool originalRefIn = false,
         originalRefOut = false;

    //==============================================================================
    /** @returns a pointer to the currently active CRC atom, or nullptr if none is configured */
    const CRCAtom* getActiveAtom() const
    {
        if (crc8 != nullptr)        return crc8.get();
        else if (crc16 != nullptr)  return crc16.get();
        else if (crc32 != nullptr)  return crc32.get();
        else if (crc64 != nullptr)  return crc64.get();

        return nullptr;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCDataUnit)
};

//==============================================================================
/** Table header component for the CRC demo.

    This component provides the column headers for the CRC data table display.
*/
class CRCTableHeaderComponent final : public TableHeaderComponent
{
public:
    CRCTableHeaderComponent()
    {
        addColumn (TRANS ("CRC Bits"),      static_cast<int> (CRCColumnID::Bits), 80);
        addColumn (TRANS ("poly"),          static_cast<int> (CRCColumnID::Poly), 120);
        addColumn (TRANS ("xorIn"),         static_cast<int> (CRCColumnID::XorIn), 80);
        addColumn (TRANS ("xorOut"),        static_cast<int> (CRCColumnID::XorOut), 80);
        addColumn (TRANS ("refIn"),         static_cast<int> (CRCColumnID::RefIn), 60);
        addColumn (TRANS ("refOut"),        static_cast<int> (CRCColumnID::RefOut), 60);
        addColumn (TRANS ("Check Value"),   static_cast<int> (CRCColumnID::CheckValue), 100);
        addColumn (TRANS ("Dec Result"),    static_cast<int> (CRCColumnID::DecResult), 100);
        addColumn (TRANS ("Hex Result"),    static_cast<int> (CRCColumnID::HexResult), 100);
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCTableHeaderComponent)
};

//==============================================================================
/** Table list box model for displaying CRC data units.

    This model manages the data for the CRC table display, including row count,
    background painting, and cell content rendering.
*/
class CRCTableListBoxModel final : public TableListBoxModel
{
public:
    CRCTableListBoxModel()
    {
        populateWithTestCases();
    }

    //==============================================================================
    /** @internal */
    int getNumRows() override   { return dataUnits.size(); }
    /** @internal */
    int getNumRows() const      { return dataUnits.size(); }
    /** @internal */
    void paintRowBackground (Graphics&, int, int, int, bool) override { }

    /** @internal */
    void paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override
    {
        SQUAREPINE_CRASH_TRACER

        if (auto* unit = dataUnits[rowNumber])
        {
            String text;
            switch (static_cast<CRCColumnID> (columnId))
            {
                case CRCColumnID::Bits:
                    text = String (unit->getNumBits());
                break;

                case CRCColumnID::Poly:
                    text = "0x" + String::toHexString (unit->getOriginalPoly());
                break;

                case CRCColumnID::XorIn:
                    text = "0x" + String::toHexString (unit->getOriginalXorIn());
                break;

                case CRCColumnID::XorOut:
                    text = "0x" + String::toHexString (unit->getOriginalXorOut());
                break;

                case CRCColumnID::RefIn:
                    text = booleanToString (unit->getOriginalRefIn());
                break;

                case CRCColumnID::RefOut:
                    text = booleanToString (unit->getOriginalRefOut());
                break;

                case CRCColumnID::CheckValue:
                    text = "0x" + String::toHexString (unit->getCheckValue());
                break;

                case CRCColumnID::DecResult:
                    text = unit->getFinalValueAsDecString();
                break;

                case CRCColumnID::HexResult:
                    text = unit->getFinalValueAsHexString();
                break;

                default:
                    text = "?";
                break;
            }

            // g.setColour (rowIsSelected ? Colours::white : Colours::black);
            g.setFont (14.0f);
            g.drawText (text, 0, 0, width, height, Justification::centredLeft, false);
        }
    }

private:
    //==============================================================================
    OwnedArray<CRCDataUnit> dataUnits;

    //==============================================================================
    /** Populates the data units with CRC test cases from the unit tests */
    void populateWithTestCases()
    {
        SQUAREPINE_CRASH_TRACER

        // CRC-8 test cases
        addTestCase (8, 0x31, 0x00, 0x00, true, true);
        addTestCase (8, 0x32, 0x00, 0xff, true, true);
        addTestCase (8, 0x07, 0xff, 0x00, true, true);
        addTestCase (8, 0x08, 0xff, 0xff, true, true);
        addTestCase (8, 0xd5, 0x00, 0x00, false, false);
        addTestCase (8, 0x49, 0x00, 0xff, false, false);
        addTestCase (8, 0x1d, 0xff, 0x00, false, false);
        addTestCase (8, 0x1d, 0xff, 0xff, false, false);
        addTestCase (8, 0x1d, 0xfd, 0x00, false, false);
        addTestCase (8, 0x1d, 0xc7, 0x00, false, false);

        // CRC-16 test cases
        addTestCase (16, 0x8005, 0x0000, 0x0000, true, true);
        addTestCase (16, 0x3d65, 0x0000, 0xffff, true, true);
        addTestCase (16, 0x1021, 0xffff, 0x0000, true, true);
        addTestCase (16, 0x1021, 0xffff, 0xffff, true, true);
        addTestCase (16, 0x0589, 0x0000, 0x0000, false, false);
        addTestCase (16, 0x0589, 0x0000, 0x0001, false, false);
        addTestCase (16, 0xc867, 0xffff, 0x0000, false, false);
        addTestCase (16, 0x3d65, 0x0000, 0xffff, false, false);
        addTestCase (16, 0x1021, 0xffff, 0xffff, false, false);

        // CRC-32 test cases
        addTestCase (32, 0x8001801b, 0x00000000, 0x00000000, true, true);
        addTestCase (32, 0xf4acfb13, 0x00000000, 0xffffffff, true, true);
        addTestCase (32, 0x04c11db7, 0xffffffff, 0x00000000, true, true);
        addTestCase (32, 0x174841bc, 0xffffffff, 0xffffffff, true, true);
        addTestCase (32, 0x814141ab, 0x00000000, 0x00000000, false, false);
        addTestCase (32, 0x04c11db7, 0x00000000, 0xffffffff, false, false);
        addTestCase (32, 0x04c11db7, 0xffffffff, 0x00000000, false, false);
        addTestCase (32, 0x04c11db7, 0xffffffff, 0xffffffff, false, false);

        // CRC-64 test cases
        addTestCase (64, 0x000000000000001B, 0xffffffffffffffff, 0xffffffffffffffff, true, true);
        addTestCase (64, 0x42f0e1eba9ea3693, 0, 0, false, false);
    }

    /** Adds a test case to the data units array */
    void addTestCase (int numBits, uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut)
    {
        if (auto* unit = dataUnits.add (new CRCDataUnit()))
            unit->configure (numBits, poly, xorIn, xorOut, refIn, refOut);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCTableListBoxModel)
};

//==============================================================================
/** Demo for CRC functionality.

    This demo provides a user interface for testing and displaying various CRC
    configurations and their results in a table format.

    @see CRCDataUnit, CRCTableListBoxModel
*/
class CRCDemo final : public DemoBase
{
public:
    CRCDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("CRC"))
    {
        SQUAREPINE_CRASH_TRACER

        tableListBox.setModel (&crcTableListBoxModel);
        tableListBox.setHeader (std::make_unique<CRCTableHeaderComponent>());
        addAndMakeVisible (tableListBox);

        updateWithNewTranslations();
    }

    ~CRCDemo() override
    {
        tableListBox.setModel (nullptr);
    }

    //==============================================================================
    /** @internal */
    void resized() override
    {
        auto b = getLocalBounds();
        tableListBox.setBounds (b);
    }

    /** @internal */
    void updateWithNewTranslations() override
    {
        SQUAREPINE_CRASH_TRACER
        tableListBox.updateContent();
        repaint();
    }

private:
    //==============================================================================
    CRCTableListBoxModel crcTableListBoxModel;
    TableListBox tableListBox;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCDemo)
};
