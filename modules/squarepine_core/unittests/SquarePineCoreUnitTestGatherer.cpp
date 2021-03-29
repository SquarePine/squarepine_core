OwnedArray<UnitTest> SquarePineCoreUnitTestGatherer::createTests()
{
    OwnedArray<UnitTest> tests;

   #if SQUAREPINE_COMPILE_UNIT_TESTS
    tests.add (new AngleUnitTests());
    tests.add (new BlumBlumShubUnitTests());
    tests.add (new ISAACUnitTests());
    tests.add (new MathsUnitTests());
    tests.add (new MovingAccumulatorTests());
    tests.add (new RandomUnitTests());
    tests.add (new SHA1Tests());
    tests.add (new XorshiftUnitTests());
   #endif

    return tests;
}
