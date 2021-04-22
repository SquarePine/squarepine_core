//============================================================================
/** A structure representing an angle in radians.
    To convert to and from degrees, see fromDegrees and toDegrees to manipulate as such.
*/
template<typename ValueType,
         typename = typename std::enable_if<std::is_floating_point<ValueType>::value>::type>
struct Angle final
{
    //==============================================================================
    /** This underlying value type for the Angle instance. */
    using Type = ValueType;

    //==============================================================================
    /** Creates an angle at 0 radians. */
    constexpr Angle() noexcept = default;

    /** Creates an angle in radians from a specified value. */
    explicit Angle (Type angleInRadians) noexcept : radians (angleInRadians) { }

    /** Creates a copy of another angle. */
    constexpr Angle (const Angle&) noexcept = default;

    //==============================================================================
    /** @returns an angle (in radians) between two 2D points. */
    template<typename FirstType, typename SecondType>
    static Angle betweenPoints (juce::Point<FirstType> a, juce::Point<SecondType> b) noexcept
    {
        return Angle (static_cast<Type> (a.toDouble().getAngleToPoint (b.toDouble())));
    }

    //==============================================================================
    /** @returns an angle in radians from a value in degrees. */
    static Angle fromDegrees (Type degrees) noexcept            { return Angle (degreesToRadians (degrees)); }

    /** @returns a conversion of radians to degrees. */
    Type toDegrees() const noexcept                             { return radiansToDegrees (radians); }

    //==============================================================================
    /** @returns an angle in radians from a value in turns. */
    static Angle fromTurns (Type turns) noexcept                { return Angle (turns * MathConstants<Type>::twoPi); }

    /** @returns a conversion of radians to turns. */
    constexpr Type toTurns() const noexcept                     { return radians / MathConstants<Type>::twoPi; }

    //==============================================================================
    /** @returns an angle in radians from a value in gradians. */
    static Angle fromGradians (Type gradians) noexcept          { return Angle (gradians * MathConstants<Type>::pi / static_cast<Type> (200)); }

    /** @returns a conversion of radians to gradians. */
    constexpr Type toGradians() const noexcept                  { return radians * static_cast<Type> (200) / MathConstants<Type>::pi; }

    //==============================================================================
    /** @returns an angle in radians from a value in arcminutes. */
    static Angle fromArcminutes (Type arcminutes) noexcept      { return Angle (arcminutes * MathConstants<Type>::pi / static_cast<Type> (60 * 180)); }

    /** @returns a conversion of radians to arcminutes. */
    constexpr Type toArcminutes() const noexcept                { return radians * static_cast<Type> (60 * 180) / MathConstants<Type>::pi; }

    //==============================================================================
    /** @returns an angle in radians from a specified value in arcseconds. */
    static Angle fromArcseconds (Type arcseconds) noexcept      { return Angle (arcseconds * MathConstants<Type>::pi / static_cast<Type> (180 * 3600)); }

    /** @returns a conversion of radians to arcseconds. */
    constexpr Type toArcseconds() const noexcept                { return radians * static_cast<Type> (3600 * 180) / MathConstants<Type>::pi; }

    //==============================================================================
    /** @returns an angle in radians based on a composition of a scaled value
        system that evenly divides a circle into sections.
    */
    static Angle fromRatioOfSections (Type value, Type degreesPerSection) noexcept
    {
        return { fromDegrees (value * degreesPerSection) };
    }

    /** @returns a ratio based on the provided degrees
        that evenly divides a circle into sections.
    */
    constexpr Type toRatioOfSections (Type degreesPerSection) const noexcept
    {
        return toDegrees() / degreesPerSection;
    }

    //==============================================================================
    /** @returns an angle in radians from a value in octants. */
    static Angle fromOctants (Type octants) noexcept            { return fromRatioOfSections (octants, static_cast<Type> (45)); }

    /** @returns a conversion of radians to octants. */
    constexpr Type toOctants() const noexcept                   { return toRatioOfSections (static_cast<Type> (45)); }

    //==============================================================================
    /** @returns an angle in radians from a value in sextants. */
    static Angle fromSextants (Type sextants) noexcept          { return fromRatioOfSections (sextants, static_cast<Type> (60)); }

    /** @returns a conversion of radians to sextants. */
    constexpr Type toSextants() const noexcept                  { return toRatioOfSections (static_cast<Type> (60)); }

    //==============================================================================
    /** @returns an angle in radians from a value in quadrants. */
    static Angle fromQuadrants (Type quadrants) noexcept        { return fromRatioOfSections (quadrants, static_cast<Type> (90)); }

    /** @returns a conversion of radians to quadrants. */
    constexpr Type toQuadrants() const noexcept                 { return toRatioOfSections (static_cast<Type> (90)); }

    //==============================================================================
    /** Copies this angle from another one. */
    Angle& operator= (const Angle&) noexcept = default;

    /** Adds two angles together */
    constexpr Angle operator+ (Angle other) const noexcept              { return Angle (radians + other.radians); }

    /** Adds another angle's coordinates to this one */
    Angle& operator+= (Angle other) noexcept                            { radians += other.radians; return *this; }

    /** Subtracts one angle from another */
    constexpr Angle operator- (Angle other) const noexcept              { return Angle (radians - other.radians); }

    /** Subtracts another angle's coordinates to this one */
    Angle& operator-= (Angle other) noexcept                            { radians -= other.radians; return *this; }

    /** Multiplies two angles together */
    template<typename OtherType>
    constexpr Angle operator* (Angle<OtherType> other) const noexcept   { return Angle ((Type) (radians * other.radians)); }

    /** Multiplies another angle's coordinates to this one */
    template<typename OtherType>
    Angle& operator*= (Angle<OtherType> other) noexcept                 { *this = *this * other; return *this; }

    /** Divides one angle by another */
    template<typename OtherType>
    constexpr Angle operator/ (Angle<OtherType> other) const noexcept   { return Angle ((Type) (radians / other.radians)); }

    /** Divides this angle's coordinates by another */
    template<typename OtherType>
    Angle& operator/= (Angle<OtherType> other) noexcept                 { *this = *this / other; return *this; }

    /** @returns a angle multiplied by a given scalar value. */
    template<typename OtherType>
    constexpr Angle operator* (OtherType multiplier) const noexcept
    {
        using CommonType = typename std::common_type<Type, OtherType>::type;
        return Angle ((Type) ((CommonType) radians * (CommonType) multiplier));
    }

    /** @returns a angle divided by a given scalar value. */
    template<typename OtherType>
    constexpr Angle operator/ (OtherType divisor) const noexcept
    {
        using CommonType = typename std::common_type<Type, OtherType>::type;
        return Angle ((Type) ((CommonType) radians / (CommonType) divisor));
    }

    /** Multiplies the angle's coordinates by a scalar value. */
    template<typename FloatType>
    Angle& operator*= (FloatType multiplier) noexcept           { radians = (Type) (radians * multiplier); return *this; }

    /** Divides the angle's coordinates by a scalar value. */
    template<typename FloatType>
    Angle& operator/= (FloatType divisor) noexcept              { radians = (Type) (radians / divisor); return *this; }

    /** @returns the inverse of this angle. */
    constexpr Angle operator-() const noexcept                  { return Angle (-radians); }

    /** @returns true if this angle matches the other one. */
    constexpr bool operator== (Angle other) const noexcept      { return approximatelyEqual (radians, other.radians); }
    /** @returns true if this angle does not match the other one. */
    constexpr bool operator!= (Angle other) const noexcept      { return ! operator== (other); }
    /** @returns */
    constexpr bool operator< (Angle other) const noexcept       { return radians < other.radians; }
    /** @returns */
    constexpr bool operator<= (Angle other) const noexcept      { return radians <= other.radians; }
    /** @returns */
    constexpr bool operator> (Angle other) const noexcept       { return radians > other.radians; }
    /** @returns */
    constexpr bool operator>= (Angle other) const noexcept      { return radians >= other.radians; }

    /** @returns */
    constexpr operator Type() const noexcept                    { return radians; }

    //==============================================================================
    /** @returns a casted version of this angle as an Angle<float>. */
    constexpr Angle<float> toFloat() const noexcept             { return Angle<float> (static_cast<float> (radians)); }

    /** @returns a casted version of this angle as an Angle<double>. */
    constexpr Angle<double> toDouble() const noexcept           { return Angle<double> (static_cast<double> (radians)); }

    /** @returns a casted version of this angle as an Angle<long double>. */
    constexpr Angle<long double> toLongDouble() const noexcept  { return Angle<long double> (static_cast<long double> (radians)); }

    /** @returns the Angle as a string. */
    String toString() const                                     { return String (radians); }

    //==============================================================================
    Type radians {}; /** The angle in radians. */
};
