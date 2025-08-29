//==============================================================================
/**
*/
class CRCDataUnit final
{
public:
    CRCDataUnit() = default;
    ~CRCDataUnit() = default;

    void configure (int numBits, uint64 poly, uint64 xorIn,
                    uint64 xorOut, bool refIn, bool refOut)
    {
    }

    uint8 getNumBits() const
    {
        if (crc8 != nullptr)        return 8;
        else if (crc16 != nullptr)  return 16;
        else if (crc32 != nullptr)  return 32;
        else if (crc64 != nullptr)  return 64;

        return 0; // Not configured... yet?
    }

    uint8 getNumBytes() const
    {
        if (const auto numBits = getNumBits(); numBits > 0)
            return numBits / 8;

        return 0; // Not configured... yet?
    }

    uint64 getCheckValue() const            { return {}; }
    String getCheckString() const           { return {}; }
    uint64 getFinalValue() const            { return {}; }
    String getFinalValueAsDecString() const { return {}; }
    String getFinalValueAsHexString() const { return {}; }
    String getFinalValueAsOctString() const { return {}; }

private:
    //==============================================================================
    struct CRCAtom
    {
        CRCAtom() = default;
        virtual ~CRCAtom() = default;

        virtual void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) = 0;

        virtual uint8 getNumBits() const = 0;
        virtual uint64 getCheckValue() const = 0;
        virtual String getCheckString() const = 0;
        virtual uint64 getFinalValue() const = 0;
        virtual String getFinalValueAsDecString() const = 0;
        virtual String getFinalValueAsHexString() const = 0;
        virtual String getFinalValueAsOctString() const = 0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCAtom)
    };

    struct CRC8Atom final : public CRCAtom
    {
        CRC8Atom() = default;

        void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) override
        {
            crc = std::make_unique<CRCType> ((Type) poly, (Type) xorIn, (Type) xorOut, refIn, refOut);
        }

        uint8 getNumBits() const override                   { return 8; }
        uint64 getCheckValue() const override               { return {}; }
        String getCheckString() const override              { return {}; }
        uint64 getFinalValue() const override               { return {}; }
        String getFinalValueAsDecString() const override    { return {}; }
        String getFinalValueAsHexString() const override    { return {}; }
        String getFinalValueAsOctString() const override    { return {}; }

        using Type = uint8;
        using CRCType = CRC<Type>;
        std::unique_ptr<CRCType> crc;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRC8Atom)
    };

    struct CRC16Atom final : public CRCAtom
    {
        CRC16Atom() = default;

        void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) override
        {
            crc = std::make_unique<CRCType> ((Type) poly, (Type) xorIn, (Type) xorOut, refIn, refOut);
        }

        uint8 getNumBits() const override                   { return 16; }
        uint64 getCheckValue() const override               { return {}; }
        String getCheckString() const override              { return {}; }
        uint64 getFinalValue() const override               { return {}; }
        String getFinalValueAsDecString() const override    { return {}; }
        String getFinalValueAsHexString() const override    { return {}; }
        String getFinalValueAsOctString() const override    { return {}; }

        using Type = uint16;
        using CRCType = CRC<Type>;
        std::unique_ptr<CRCType> crc;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRC16Atom)
    };

    struct CRC32Atom final : public CRCAtom
    {
        CRC32Atom() = default;

        void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) override
        {
            crc = std::make_unique<CRCType> ((Type) poly, (Type) xorIn, (Type) xorOut, refIn, refOut);
        }

        uint8 getNumBits() const override                   { return 32; }
        uint64 getCheckValue() const override               { return {}; }
        String getCheckString() const override              { return {}; }
        uint64 getFinalValue() const override               { return {}; }
        String getFinalValueAsDecString() const override    { return {}; }
        String getFinalValueAsHexString() const override    { return {}; }
        String getFinalValueAsOctString() const override    { return {}; }

        using Type = uint32;
        using CRCType = CRC<Type>;
        std::unique_ptr<CRCType> crc;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRC32Atom)
    };

    struct CRC64Atom final : public CRCAtom
    {
        CRC64Atom() = default;

        void configure (uint64 poly, uint64 xorIn, uint64 xorOut, bool refIn, bool refOut) override
        {
            crc = std::make_unique<CRCType> ((Type) poly, (Type) xorIn, (Type) xorOut, refIn, refOut);
        }

        uint8 getNumBits() const override                   { return 64; }
        uint64 getCheckValue() const override               { return {}; }
        String getCheckString() const override              { return {}; }
        uint64 getFinalValue() const override               { return {}; }
        String getFinalValueAsDecString() const override    { return {}; }
        String getFinalValueAsHexString() const override    { return {}; }
        String getFinalValueAsOctString() const override    { return {}; }

        using Type = uint64;
        using CRCType = CRC<Type>;
        std::unique_ptr<CRCType> crc;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRC64Atom)
    };

    std::unique_ptr<CRC8Atom> crc8;
    std::unique_ptr<CRC16Atom> crc16;
    std::unique_ptr<CRC32Atom> crc32;
    std::unique_ptr<CRC64Atom> crc64;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCDataUnit)
};

//==============================================================================
/**
*/
class CRCTableHeaderComponent final : public TableHeaderComponent
{
public:
    CRCTableHeaderComponent()
    {
    }

private:
    //==============================================================================
    

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCTableHeaderComponent)
};

//==============================================================================
/**
*/
class CRCTableListBoxModel final : public TableListBoxModel
{
public:
    CRCTableListBoxModel()
    {
    }

    //==============================================================================
    int getNumRows() override
    {
        return dataUnits.size();
    }

    void paintRowBackground (Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override
    {
    }

    void paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override
    {
    }

private:
    //==============================================================================
    OwnedArray<CRCDataUnit> dataUnits;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCTableListBoxModel)
};

//==============================================================================
/**
*/
class CRCDemo final : public DemoBase
{
public:
    CRCDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("CRC"))
    {
        {
            CRC<uint8> crcTest (0x07);
            const auto r = (uint8) crcTest.processByte (0x61).get();
            // jassert (r == 0x36);
        }

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
    void resized() override
    {
        auto b = getLocalBounds();
        tableListBox.setBounds (b);
    }

    void updateWithNewTranslations() override
    {
        SQUAREPINE_CRASH_TRACER
        tableListBox.updateContent();
        repaint();
    }

    //==============================================================================

private:
    //==============================================================================
    CRCTableListBoxModel crcTableListBoxModel;
    TableListBox tableListBox;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CRCDemo)
};
