//==============================================================================
/** Instead of globally and statically creating unit tests,
    which can bias your tests into all sorts of different and undesirable corners,
    subclass from this unit test gatherer to be able to populate an organised
    list of tests.
*/
class UnitTestGatherer
{
public:
    /** Constructor. */
    UnitTestGatherer() = default;

    /** Destructor. */
    virtual ~UnitTestGatherer() = default;

    //==============================================================================
    /** @returns a list of unit tests that can be run at a later time,
        therefore in some explicitly controlled fashion by the user.
    */
    virtual OwnedArray<UnitTest> createTests() = 0;

    /** Convenience method to help gather many unit tests into a single place. */
    void appendUnitTests (OwnedArray<UnitTest>& destinationUnitTests);

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnitTestGatherer)
};

//==============================================================================
/** Assembles all unit tests for the SquarePine Core module. */
class SquarePineCoreUnitTestGatherer final : public UnitTestGatherer
{
public:
    /** Constructor. */
    SquarePineCoreUnitTestGatherer() = default;

    //==============================================================================
    /** @internal */
    OwnedArray<UnitTest> createTests() override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquarePineCoreUnitTestGatherer)
};
