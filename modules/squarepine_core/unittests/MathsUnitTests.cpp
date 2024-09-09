#if SQUAREPINE_COMPILE_UNIT_TESTS

//==============================================================================
class MovingAccumulatorTests final : public UnitTest
{
public:
    MovingAccumulatorTests() : UnitTest ("MovingAccumulator", UnitTestCategories::maths) {}

    void runTest()
    {
        beginTest ("MovingAccumulator");

        MovingAccumulator<double> average;
        average.function = calculateNextAverage<double>;

        expectEquals (average.add (1.0), 1.0);
        expectEquals (average.add (2.0), 1.5);
        expectEquals (average.add (3.0), 2.0);
        expectEquals (average.add (4.0), 2.5);
        expectEquals (average.add (5.0), 3.0);

        expectEquals (average.getNumValues(), 5);
    }
};

//==============================================================================
class MathsUnitTests final : public UnitTest
{
public:
    MathsUnitTests() : UnitTest ("Maths Utilities", UnitTestCategories::maths) {}

    void runTest()
    {
        beginTest ("Maths Utilities");

        // RMS
        {
            constexpr auto numSamples = 512;
            juce::AudioBuffer<float> asb (1, numSamples);

            // Sin
            {
                const auto delta = numSamples / MathConstants<float>::twoPi;
                auto* data = asb.getWritePointer (0);

                for (int i = 0; i < numSamples; ++i)
                    *data++ = std::sin ((float) i * delta);

                const auto rms = findRMS (asb.getReadPointer (0), numSamples);
                expect (approximatelyEqual (rms, 0.707804799f));
            }

            // Square
            {
                auto* data = asb.getWritePointer (0);

                for (int i = 0; i < numSamples; ++i)
                    *data++ = isOdd (i) ? 1.0f : -1.0f;

                const auto rms = findRMS (asb.getReadPointer (0), numSamples);
                expect (approximatelyEqual (rms, 1.0f));
            }
        }
    }
};

#endif // SQUAREPINE_COMPILE_UNIT_TESTS
