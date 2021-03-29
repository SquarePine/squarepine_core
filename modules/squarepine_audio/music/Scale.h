/** */
class Scale final
{
public:
    /** */
    enum class Type
    {
        major = 0,
        minor,
        ionian,
        dorian,
        phrygian,
        lydian,
        mixolydian,
        aeolian,
        locrian,
        melodicMinor,
        harmonicMinor
    };

    /** */
    enum class StepType
    {
        whole = 0,
        half,
        wholeHalf
    };

    /** */
    enum class IntervalType
    {
        i = 0,
        ii,
        iii,
        iv,
        v,
        vi,
        vii
    };

    //==============================================================================
    /** */
    Scale (Type type = Type::major);

    //==============================================================================
    /** */
    Type getType() const noexcept { return type; }

    /** */
    String getName() const;
    /** */
    String getShortName() const;

    /** */
    Array<int> getSteps (int octaves = 1) const;

    /** */
    String getIntervalName (IntervalType interval) const;

    /** */
    const Array<Chord>& getTriads() const noexcept { return triads; }
    /** */
    const Array<Chord>& getSixths() const noexcept { return sixths; }
    /** */
    const Array<Chord>& getSevenths() const noexcept { return sevenths; }

    //==============================================================================
    /** */
    static StringArray getIntervalNames();
    /** */
    static Array<Type> getAllScaleTypes();
    /** */
    static StringArray getScaleStrings();
    /** */
    static String getNameForType (Type type);
    /** */
    static String getShortNameForType (Type type);
    /** */
    static Type getTypeFromName (const String& name);

private:
    //==============================================================================
    Type type;
    Array<StepType> steps;
    Array<Chord> triads, sixths, sevenths;

    //==============================================================================
    Array<Chord> generateTriads (int offset) const;
    Array<Chord> generateSixths (int offset) const;
    Array<Chord> generateSevenths (int offset) const;

    //==============================================================================
    JUCE_LEAK_DETECTOR (Scale)
};
