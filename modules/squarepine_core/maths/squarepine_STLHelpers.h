namespace std
{
   #if ! JUCE_CXX20_IS_AVAILABLE
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
   #endif // JUCE_CXX20_IS_AVAILABLE
}
