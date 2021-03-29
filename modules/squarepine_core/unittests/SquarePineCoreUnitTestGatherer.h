/** */
class UnitTestGatherer
{
public:
    /** */
    UnitTestGatherer() = default;

    /** */
    virtual ~UnitTestGatherer() = default;

    //==============================================================================
    /** */
    virtual OwnedArray<UnitTest> createTests() = 0;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnitTestGatherer)
};

//==============================================================================
/** */
class SquarePineCoreUnitTestGatherer final : public UnitTestGatherer
{
public:
    /** */
    SquarePineCoreUnitTestGatherer() = default;

    //==============================================================================
    /** @internal */
    OwnedArray<UnitTest> createTests() override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquarePineCoreUnitTestGatherer)
};
