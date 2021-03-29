//==============================================================================
/** */
template<typename ValueType>
struct ControlPoint3D final
{
    /** */
    using Type = ValueType;

    /** */
    ControlPoint3D() noexcept = default;
    /** */
    ControlPoint3D (const ControlPoint3D&) noexcept = default;
    /** */
    ControlPoint3D (ControlPoint3D&&) noexcept = default;
    /** */
    ~ControlPoint3D() noexcept = default;
    /** */
    ControlPoint3D& operator= (const ControlPoint3D&) noexcept = default;
    /** */
    ControlPoint3D& operator= (ControlPoint3D&&) noexcept = default;

    /** */
    ControlPoint3D (Type a, Type b, Type c = Type()) noexcept : x (a), y (b), z (c) { }

    /** */
    Type operator[] (int index) const noexcept
    {
        if (index == 0)         return x;
        else if (index == 1)    return y;
        else if (index == 2)    return z;

        jassertfalse;
        return Type();
    }

    /** */
    void set (int index, Type value) noexcept
    {
        if (index == 0)         { x = value; }
        else if (index == 1)    { y = value; }
        else if (index == 2)    { z = value; }
        else                    { jassertfalse; }
    }

    Type x = 0, y = 0, z = 0;
};

//==============================================================================
/** */
class CubicBezierInterpolator final
{
public:
    /** */
    using Type = double;
    /** */
    using Point = Point<Type>;
    /** */
    using ControlPoint = ControlPoint3D<Type>;

    /** */
    CubicBezierInterpolator (double x1, double y1, double x2, double y2) noexcept :
        controlPoints ({ ControlPoint(), { x1, y1 }, { x2, y2 }, { 1.0, 1.0, 1.0 } })
    {
        for (int i = 0; i < 3; ++i)
        {
            const auto c0 = controlPoints[0][i];
            const auto c1 = controlPoints[1][i];
            const auto c2 = controlPoints[2][i];
            const auto c3 = controlPoints[3][i];

            polinomials[0].set (i, c0 + 3.0 * (c1 - c2) + c3);
            polinomials[1].set (i, 3.0 * (c0 - 2.0 * c1 + c2));
            polinomials[2].set (i, 3.0 * (-c0 + c1));
            polinomials[3].set (i, c0);
        }
    }

    /** */
    CubicBezierInterpolator (const Point& a, const Point& b) noexcept :
        CubicBezierInterpolator (a.x, a.y, b.x, b.y)
    {
    }

    /** */
    double getWeightedValue (double weight) const noexcept
    {
        weight = std::clamp (weight, 0.0, 1.0);

        auto l = 0.0, u = 1.0, s = 0.0;
        auto recalculateS = [&]()
        {
            s = (l + u) * 0.5;
        };

        recalculateS();
        auto x = getX (s);

        while (std::abs (weight - x) > 0.0001)
        {
            if (weight > x)
                l = s;
            else
                u = s;

            recalculateS();
            x = getX (s);
        }

        return getY (s);
    }

    /** */
    double getX (double weight) const noexcept { return getValue (0, weight); }
    /** */
    double getY (double weight) const noexcept { return getValue (1, weight); }
    /** */
    double getZ (double weight) const noexcept { return getValue (2, weight); }

private:
    const std::array<ControlPoint, 4> controlPoints;
    std::array<ControlPoint, 4> polinomials;

    forcedinline double getValue (int i, double weight) const noexcept
    {
        jassert (isPositiveAndBelow (i, 4));

        weight = std::clamp (weight, 0.0, 1.0);
        return ((polinomials[0][i] * weight + polinomials[1][i]) * weight + polinomials[2][i]) * weight
                + polinomials[3][i];
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CubicBezierInterpolator)
};

/** */
template<typename Type>
inline Type cubicBezierInterpolation (Type a, Type b, Type c, Type d, Type weight)
{
    return (Type) CubicBezierInterpolator ((double) a, (double) b, (double) c, (double) d)
                    .getWeightedValue ((double) weight);
}

//==============================================================================
/**
    @see https://www.intmath.com/blog/mathematics/eulers-method-runge-kutta-rk4-10117
    @see https://adamwermus.wordpress.com/2016/01/22/the-mass-spring-model-parker-sochacki-vs-runge-kutta-4/
    @see http://hplgit.github.io/Programming-for-Computations/pub/p4c/._p4c-solarized-Matlab021.html
*/
class MassSpringModel final
{
public:
    /** */
    static double calculateRK4 (double /*weight*/,
                                double position,
                                double velocity,
                                double spring = 5.0,
                                double damping = 4.0,
                                double mass = 5.0) noexcept
    {
        const auto dv = (-damping * velocity - spring * position) / mass;

        return dv;
    }

private:
    MassSpringModel() = delete;
};

//==============================================================================
namespace ease
{
    namespace spring
    {
        namespace in
        {
        }

        namespace out
        {
        }

        namespace inOut
        {
        }
    }
}
