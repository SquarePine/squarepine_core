/** Assembles all unit tests for the SquarePine Cryptography module. */
class SquarePineCryptographyUnitTestGatherer final : public UnitTestGatherer
{
public:
    /** Constructor. */
    SquarePineCryptographyUnitTestGatherer() = default;

    //==============================================================================
    /** @internal */
    OwnedArray<UnitTest> createTests() override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquarePineCryptographyUnitTestGatherer)
};
