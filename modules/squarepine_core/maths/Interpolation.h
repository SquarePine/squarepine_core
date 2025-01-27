//==============================================================================
/** */
[[nodiscard]] constexpr float lerp (float a, float b, float weight) noexcept
{
    return (a * (1.0f - weight)) + (b * weight);
}

/** */
[[nodiscard]] constexpr double lerp (double a, double b, double weight) noexcept
{
    return (a * (1.0 - weight)) + (b * weight);
}

/** */
template<typename Type>
[[nodiscard]] constexpr Type lerp (Type a, Type b, Type weight) noexcept
{
    return (a * (static_cast<Type> (1) - weight)) + (b * weight);
}

//==============================================================================
/** */
[[nodiscard]] constexpr float smoothstep (float a, float b, float weight) noexcept
{
    const auto w = std::clamp ((weight - a) / (b - a), 0.0f, 1.0f);
    return square (w) * (3.0f - (2.0f * w));
}

/** */
[[nodiscard]] constexpr double smoothstep (double a, double b, double weight) noexcept
{
    const auto w = std::clamp ((weight - a) / (b - a), 0.0, 1.0);
    return square (w) * (3.0 - (2.0 * w));
}

/** */
template<typename Type>
[[nodiscard]] constexpr Type smoothstep (Type a, Type b, Type weight) noexcept
{
    return (Type) smoothstep ((double) a, (double) b, (double) weight);
}

//==============================================================================
/** */
[[nodiscard]] constexpr float smootherstep (float a, float b, float weight) noexcept
{
    const auto w = std::clamp ((weight - a) / (b - a), 0.0f, 1.0f);
    return cube (w) * (w * (w * 6.0f - 15.0f) + 10.0f);
}

/** */
[[nodiscard]] constexpr double smootherstep (double a, double b, double weight) noexcept
{
    const auto w = std::clamp ((weight - a) / (b - a), 0.0, 1.0);
    return cube (w) * (w * (w * 6.0 - 15.0) + 10.0);
}

/** */
template<typename Type>
[[nodiscard]] constexpr Type smootherstep (Type a, Type b, Type weight) noexcept
{
    return (Type) smootherstep ((double) a, (double) b, (double) weight);
}

//==============================================================================
/** */
[[nodiscard]] inline float inverseSmoothstep (float x) noexcept
{
    return 0.5f - std::sin (std::asin (1.0f - 2.0f * x) / 3.0f);
}

/** */
[[nodiscard]] inline double inverseSmoothstep (double x) noexcept
{
    return 0.5 - std::sin (std::asin (1.0 - 2.0 * x) / 3.0);
}
