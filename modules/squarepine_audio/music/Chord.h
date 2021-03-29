/** */
class Chord final
{
public:
    /** */
    enum class Type
    {
        custom = -2,
        invalid,
        majorTriad,
        minorTriad,
        diminishedTriad,
        augmentedTriad,
        majorSixth,
        minorSixth,
        dominantSeventh,
        majorSeventh,
        minorSeventh,
        augmentedSeventh,
        diminishedSeventh,
        halfDiminishedSeventh,
        minorMajorSeventh,
        suspendedSecond,
        suspendedFourth,
        power,
        majorNinth,
        dominantNinth,
        minorMajorNinth,
        minorDominantNinth,
        augmentedMajorNinth,
        augmentedDominantNinth,
        halfDiminishedNinth,
        halfDiminishedMinorNinth,
        diminishedNinth,
        diminishedMinorNinth
    };

    //==============================================================================
    /** */
    Chord() noexcept = default;
    /** */
    Chord (Type type) noexcept;
    /** Creates a custom chord with steps. */
    Chord (Array<int> steps, const String& symbol);
    /** */
    Chord (const Chord&) = default;
    /** */
    Chord (Chord&&) = default;
    /** */
    Chord& operator= (const Chord&) = default;
    /** */
    Chord& operator= (Chord&&) = default;
    /** */
    ~Chord() = default;

    //==============================================================================
    /** */
    String toString() const;
    /** */
    static Chord fromString (const String&);

    /** */
    static Array<Type> getAllChordTypes();

    /** */
    Type getType() const noexcept { return type; }
    /** */
    bool isValid() const noexcept { return type != Type::invalid; }

    //==============================================================================
    /** */
    String getName() const;
    /** */
    String getShortName() const;
    /** */
    String getSymbol() const;
    /** */
    Array<int> getSteps() const;
    /** */
    Array<int> getSteps (int inversion) const;

private:
    //==============================================================================
    Type type = Type::invalid;
    Array<int> steps;
    String symbol;

    //==============================================================================
    JUCE_LEAK_DETECTOR (Chord)
};
