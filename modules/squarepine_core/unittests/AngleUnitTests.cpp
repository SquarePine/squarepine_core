#if SQUAREPINE_COMPILE_UNIT_TESTS

class AngleUnitTests final : public UnitTest
{
public:
    AngleUnitTests() :
        UnitTest ("Angle", UnitTestCategories::maths)
    {
    }

    void runTest() override
    {
        runTypeTest<float> ("Float");
        runTypeTest<double> ("Double");
        runTypeTest<long double> ("Long Double");
    }

private:
    template<typename Type>
    void runTypeTest (const String& name)
    {
        beginTest (name);

        constexpr auto valueToExpect = static_cast<Type> (65536.0065536);

        Angle<Type> a;
        a = Angle<Type>::fromDegrees (valueToExpect);
        expect (approximatelyEqual (a.toDegrees(), valueToExpect));

        a = Angle<Type>::fromTurns (valueToExpect);
        expect (approximatelyEqual (a.toTurns(), valueToExpect));

        a = Angle<Type>::fromGradians (valueToExpect);
        expect (approximatelyEqual (a.toGradians(), valueToExpect));

        a = Angle<Type>::fromArcminutes (valueToExpect);
        expect (approximatelyEqual (a.toArcminutes(), valueToExpect));

        a = Angle<Type>::fromArcseconds (valueToExpect);
        expect (approximatelyEqual (a.toArcseconds(), valueToExpect));

        a = Angle<Type>::fromOctants (valueToExpect);
        expect (approximatelyEqual (a.toOctants(), valueToExpect));

        a = Angle<Type>::fromSextants (valueToExpect);
        expect (approximatelyEqual (a.toSextants(), valueToExpect));

        a = Angle<Type>::fromQuadrants (valueToExpect);
        expect (approximatelyEqual (a.toQuadrants(), valueToExpect));
    }
};

#endif
