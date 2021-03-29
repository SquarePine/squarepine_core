/** Equations to calculate the shape of an ellipse and to calculate points
    on the edge of the ellipse and to see if points are inside or outside
*/
template<typename ValueType>
class Ellipse
{
public:
    //==============================================================================
    /** */
    using Type = ValueType;

    /** This type will be double if the Ellipse's type is double, otherwise it will be float. */
    using FloatType = typename TypeHelpers::SmallestFloatType<ValueType>::type;

    /** */
    static constexpr auto defaultAccuracy = static_cast<FloatType> (0.00001);

    //==============================================================================
    /** Creates an ellipse at the origin */
    constexpr Ellipse() = default;

    /** Creates a copy of another ellipse. */
    constexpr Ellipse (const Ellipse&) = default;

    /** Creates an ellipse from an (a, b) position. */
    constexpr Ellipse (Type initialA, Type initialB) noexcept : a (initialA), b (initialB) {}

    //==============================================================================
    /** Copies this ellipse from another one. */
    Ellipse& operator= (const Ellipse&) = default;

    /** */
    constexpr bool operator== (Ellipse other) const noexcept { return a == other.a && b == other.b; }
    /** */
    constexpr bool operator!= (Ellipse other) const noexcept { return a != other.a || b != other.b; }

    //==============================================================================
    /** @returns the cross-product. */
    constexpr FloatType getCrossProduct (Type x, Type y) const noexcept
    {
        return (square ((FloatType) a) / square ((FloatType) x))
             + (square ((FloatType) b) / square ((FloatType) y));
    }

    /** @returns the cross-product. */
    constexpr FloatType getCrossProduct (Point<Type> other) const noexcept
    {
        return getCrossProduct (other.x, other.y);
    }

    //==============================================================================
    /** */
    bool isPointOn (Type x, Type y, FloatType accuracy = defaultAccuracy) const noexcept
    {
        return std::abs (static_cast<FloatType> (1) - getCrossProduct (x, y)) < accuracy;
    }

    /** */
    bool isPointOn (Point<Type> point, FloatType accuracy = defaultAccuracy) const noexcept
    {
        return isPointOn (point.x, point.y, accuracy);
    }

    //==============================================================================
    /** */
    bool isPointOutside (Type x, Type y) const noexcept
    {
        return getCrossProduct (x, y) > static_cast<FloatType> (1);
    }

    /** */
    bool isPointOutside (Point<Type> point) const noexcept
    {
        return isPointOutside (point.x, point.y);
    }

    //==============================================================================
    /** */
    bool isPointInside (Type x, Type y) const noexcept
    {
        return getCrossProduct (x, y) < static_cast<FloatType> (1);
    }

    /** */
    bool isPointInside (Point<Type> point) const noexcept
    {
        return isPointInside (point.x, point.y);
    }

    //==============================================================================
    /** */
    Point<FloatType> getPointAtAngle (FloatType angle) const
    {
        constexpr auto zero = static_cast<FloatType> (0);
        constexpr auto three = static_cast<FloatType> (3);
        constexpr auto twoPi = MathConstants<FloatType>::twoPi;
        constexpr auto fourPi = twoPi * static_cast<FloatType> (2);

        const auto dangle = square (std::tan (angle));
        auto x = (a * b) / std::sqrt (square (b) + square (a) * dangle);
        auto y = (a * b) / std::sqrt (square (a) + square (b) / dangle);

        while (angle < zero)
            angle += fourPi;

        angle = std::fmod (angle, fourPi);

        if (angle >= (fourPi * three))
        {
            y = -y;
        }
        else if (angle >= twoPi)
        {
            y = -y;
            x = -x;
        }
        else if (angle >= fourPi)
        {
            x = -x;
        }

        return { x, y };
    }

    //==============================================================================
    Type a {}, b {};
};
