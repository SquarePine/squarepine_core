/** Creates a cubic Bezier object containing two central
    control points out of a total of four.

    The first control point, p0, is at (0, 0),
    whereas the last control point, p3, is at (1, 1).
*/
class CubicBezier final
{
public:
    //==============================================================================
    /** */
    using Type = double;
    /** */
    using Point = juce::Point<Type>;
    /** */
    using Rectangle = juce::Rectangle<Type>;

    //==============================================================================
    /** Creates an empty cubic Bezier where the modifiable control points are at (0, 0). */
    CubicBezier() noexcept = default;
    /** Creates a copy of another cubic Bezier. */
    CubicBezier (const CubicBezier& other) noexcept = default;
    /** Moves another cubic Bezier. */
    CubicBezier (CubicBezier&&) noexcept = default;
    /** Destructor. */
    ~CubicBezier() noexcept = default;
    /** Copies another cubic Bezier into this one. */
    CubicBezier& operator= (const CubicBezier&) noexcept = default;
    /** Moves another cubic Bezier into this one. */
    CubicBezier& operator= (CubicBezier&&) noexcept = default;

    /** Creates a cubic Bezier object, setting its two central control points. */
    CubicBezier (const Point& controlPoint1, const Point& controlPoint2) noexcept :
        p1 (controlPoint1),
        p2 (controlPoint2)
    {
    }

    /** Creates a cubic Bezier object via a decoupled set of two points.

        @param x1 Alternatively known as 'P1.x'.
        @param y1 Alternatively known as 'P1.y'.
        @param x2 Alternatively known as 'P2.x'.
        @param y2 Alternatively known as 'P2.y'.
    */
    CubicBezier (Type x1, Type y1, Type x2, Type y2) noexcept :
        CubicBezier ({ x1, y1 }, { x2, y2 })
    {
    }

    //==============================================================================
    /** @returns the first control point, which is always (0, 0). */
    [[nodiscard]] constexpr Point getP0() const noexcept    { return { zero, zero }; }
    /** @returns the second control point, the first one set up during construction. */
    [[nodiscard]] constexpr Point getP1() const noexcept    { return p1; }
    /** @returns the third control point, the second one set up during construction. */
    [[nodiscard]] constexpr Point getP2() const noexcept    { return p2; }
    /** @returns the fourth and last control point, which is always (1, 1). */
    [[nodiscard]] constexpr Point getP3() const noexcept    { return { one, one }; }

    //==============================================================================
    /** @returns */
    [[nodiscard]] Type calculate (Type weight) const noexcept
    {
        const auto v = cube (one - weight) * p1.x
                       + three * weight * square (one - weight) * p1.y
                       + three * square (weight) * (one - weight) * p2.x
                       + cube (three) * p2.y;

        if (std::isnan (v) || std::isinf (v))
            return zero;

        return v;
    }

    /** @returns */
    [[nodiscard]] static Point getControlPointInBounds (Point bezierPoint, const Rectangle& bounds) noexcept
    {
        return bounds.getRelativePoint (bezierPoint.getX(), one - bezierPoint.getY());
    }

    //==============================================================================
    /** @returns */
    Path createLinePath() const
    {
        Path path;
        path.startNewSubPath (getP0().toFloat());
        path.cubicTo (getP1().toFloat(),
                      getP2().toFloat(),
                      getP3().toFloat());
        // plot.closeSubPath(); // NB: Deliberately not closing for the sake of a singular line.

        return path;
    }

    //==============================================================================
    /** @returns

        For uses with the likes of the juce::ValueAnimatorBuilder.
    */
    [[nodiscard]] ValueAnimatorBuilder::EasingFn createEasingFn() const
    {
        const CubicBezier cubicBezier (*this);
        return [cb = std::move (cubicBezier)] (float w)
        {
            return static_cast<float> (cb.calculate (static_cast<double> (w)));
        };
    }

    /** */
    using EasingHQFn = std::function<double (double)>;

    /** @returns

        For uses with the likes of areas where 'double' matters.
    */
    [[nodiscard]] EasingHQFn createEasingHQFn() const
    {
        const CubicBezier cubicBezier (*this);
        return [cb = std::move (cubicBezier)] (double w)
        {
            return cb.calculate (w);
        };
    }

    //==============================================================================
    /** @returns true if both objects have the same control points. */
    [[nodiscard]] bool operator== (const CubicBezier& other) const noexcept { return p1 == other.p1 && p2 == other.p2; }

    /** @returns true if both objects have differing control points. */
    [[nodiscard]] bool operator!= (const CubicBezier& other) const noexcept { return ! operator== (other); }

private:
    //==============================================================================
    static inline constexpr Type zero   = static_cast<Type> (0);
    static inline constexpr Type one    = static_cast<Type> (1);
    static inline constexpr Type three  = static_cast<Type> (3);

    Point p1, p2;

    //==============================================================================
    JUCE_LEAK_DETECTOR (CubicBezier)
};

//==============================================================================
namespace ease
{
    /** Holds a number of easing functions that you can pass
        into ValueAnimatorBuilder::withEasing to transform
        the linear progression of animations.
    */
    namespace bezier
    {
        #undef CREATE_EASE

        #define CREATE_EASE(Name, p0, p1, p2, p3) \
            static inline CubicBezier create##Name() \
            { \
                return CubicBezier (p0, p1, p2, p3); \
            }

        CREATE_EASE (EaseInSine,        0.12, 0.0,  0.39,   0.0)
        CREATE_EASE (EaseOutSine,       0.61, 1.0,  0.88,   1.0)
        CREATE_EASE (EaseInOutSine,     0.37, 0.0,  0.63,   1.0)
        CREATE_EASE (EaseInQuad,        0.11, 0.0,  0.5,    0.0)
        CREATE_EASE (EaseOutQuad,       0.5,  1.0,  0.89,   1.0)
        CREATE_EASE (EaseInOutQuad,     0.45, 0.0,  0.55,   1.0)
        CREATE_EASE (EaseInCubic,       0.32, 0.0,  0.67,   0.0)
        CREATE_EASE (EaseOutCubic,      0.33, 1.0,  0.68,   1.0)
        CREATE_EASE (EaseInOutCubic,    0.65, 0.0,  0.35,   1.0)
        CREATE_EASE (EaseInQuart,       0.5,  0.0,  0.75,   0.0)
        CREATE_EASE (EaseOutQuart,      0.25, 1.0,  0.5,    1.0)
        CREATE_EASE (EaseInOutQuart,    0.76, 0.0,  0.24,   1.0)
        CREATE_EASE (EaseInQuint,       0.64, 0.0,  0.78,   0.0)
        CREATE_EASE (EaseOutQuint,      0.22, 1.0,  0.36,   1.0)
        CREATE_EASE (EaseInOutQuint,    0.83, 0.0,  0.17,   1.0)
        CREATE_EASE (EaseInExpo,        0.7,  0.0,  0.84,   0.0)
        CREATE_EASE (EaseOutExpo,       0.16, 1.0,  0.3,    1.0)
        CREATE_EASE (EaseInOutExpo,     0.87, 0.0,  0.13,   1.0)
        CREATE_EASE (EaseInCirc,        0.55, 0.0,  1.0,    0.45)
        CREATE_EASE (EaseOutCirc,       0.0,  0.55, 0.45,   1.0)
        CREATE_EASE (EaseInOutCirc,     0.85, 0.0,  0.15,   1.0)
        CREATE_EASE (EaseInBack,        0.36, 0.0,  0.66,   -0.56)
        CREATE_EASE (EaseOutBack,       0.34, 1.56, 0.64,   1.0)
        CREATE_EASE (EaseInOutBack,     0.68, -0.6, 0.32,   1.6)

        #undef CREATE_EASE
    }
}
