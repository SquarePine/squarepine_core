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
    [[nodiscard]] String toString() const;
    /** */
    static Chord fromString (const String&);

    /** */
    static Array<Type> getAllChordTypes();

    /** */
    [[nodiscard]] Type getType() const noexcept { return type; }
    /** */
    [[nodiscard]] bool isValid() const noexcept { return type != Type::invalid; }

    //==============================================================================
    /** @returns */
    [[nodiscard]] String getName() const;
    /** @returns */
    [[nodiscard]] String getShortName() const;
    /** @returns */
    [[nodiscard]] String getSymbol() const;
    /** @returns */
    [[nodiscard]] Array<int> getSteps() const;
    /** @returns */
    [[nodiscard]] Array<int> getSteps (int inversion) const;

private:
    //==============================================================================
    Type type = Type::invalid;
    Array<int> steps;
    String symbol;

    //==============================================================================
    JUCE_LEAK_DETECTOR (Chord)
};
