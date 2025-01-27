/** */
template<typename FloatType = double>
class CubicBezier final
{
public:
    //==============================================================================
    /** */
    using Type = FloatType;
    /** */
    using Point = juce::Point<Type>;

    //==============================================================================
    /** */
    CubicBezier() noexcept = default;
    /** */
    CubicBezier (const CubicBezier&) noexcept = default;
    /** */
    CubicBezier (CubicBezier&&) noexcept = default;
    /** */
    ~CubicBezier() noexcept = default;
    /** */
    CubicBezier& operator= (const CubicBezier&) noexcept = default;
    /** */
    CubicBezier& operator= (CubicBezier&&) noexcept = default;

    /** Create a cubic Bezier object via its raw indices,
        where it's in fact a decoupled set of two points.

        @param x1 Alternatively known as P0.
        @param y1 Alternatively known as P1.
        @param x2 Alternatively known as P2.
        @param y2 Alternatively known as P3.
    */
    CubicBezier (Type x1, Type y1, Type x2, Type y2) noexcept :
        rawPoints ({ x1, y1, x2, y2 })
    {
    }

    /** Create a cubic Bezier object via two control points. */
    CubicBezier (const Point& a, const Point& b) noexcept :
        CubicBezier (a.x, a.y, b.x, b.y)
    {
    }

    //==============================================================================
    /** @returns */
    [[nodiscard]] constexpr Type getP0() const noexcept { return rawPoints[0]; }
    /** @returns */
    [[nodiscard]] constexpr Type getP1() const noexcept { return rawPoints[1]; }
    /** @returns */
    [[nodiscard]] constexpr Type getP2() const noexcept { return rawPoints[2]; }
    /** @returns */
    [[nodiscard]] constexpr Type getP3() const noexcept { return rawPoints[3]; }

    /** @returns */
    [[nodiscard]] constexpr Type getPointA() const noexcept { return { getP0(), getP1() }; }
    /** @returns */
    [[nodiscard]] constexpr Type getPointB() const noexcept { return { getP2(), getP3() }; }

    //==============================================================================
    /** @returns */
    [[nodiscard]] Type calculate (Type weight) const noexcept
    {
        constexpr auto one      = static_cast<Type> (1);
        constexpr auto three    = static_cast<Type> (3);

        return cube (one - weight) * getP0()
             + three * weight * square (one - weight) * getP1()
             + three * square (weight) * (one - weight) * getP2()
             + cube (three) * getP3();
    }

private:
    //==============================================================================
    std::array<Type, 4> rawPoints;

    //==============================================================================
    JUCE_LEAK_DETECTOR (CubicBezier)
};
