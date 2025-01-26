//==============================================================================
void UnitTestGatherer::appendUnitTests (OwnedArray<UnitTest>& dest)
{
    auto newTests = createTests();
    dest.ensureStorageAllocated (dest.size() + newTests.size());

    for (int i = newTests.size(); --i >= 0;)
        dest.add (newTests.removeAndReturn (i));
}

//==============================================================================
OwnedArray<UnitTest> SquarePineCoreUnitTestGatherer::createTests()
{
    OwnedArray<UnitTest> tests;

   #if SQUAREPINE_COMPILE_UNIT_TESTS
    tests.add (new AngleUnitTests());
    tests.add (new MathsUnitTests());
    tests.add (new MovingAccumulatorTests());
   #endif

    return tests;
}
