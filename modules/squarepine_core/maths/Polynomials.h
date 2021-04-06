//==============================================================================
/** @returns the result of the linear function.
    Useful for finding the slope of something along two axis.
*/
template<typename NumericType>
constexpr NumericType linearFunction (NumericType m, NumericType x, NumericType b) noexcept 
{
    return (m * x) + b;
}

/** @returns the result of a polynomial of the second degree. */
template<typename Type>
constexpr Type quadraticFunction (Type a, Type b, Type c, Type x) noexcept
{
    return (a * square (x))
         + (b * x)
         + c;
}

/** @returns the result of a polynomial of the third degree. */
template<typename Type>
constexpr Type cubicFunction (Type a, Type b, Type c, Type d, Type weight) noexcept
{
    return (a * cube (weight))
         + quadraticFunction (b, c, d, weight);
}

/** @returns the result of a polynomial of the fourth degree. */
template<typename Type>
constexpr Type quarticFunction (Type a, Type b, Type c, Type d, Type e, Type weight) noexcept
{
    return (a * biquadrate (weight))
         + cubicFunction (b, c, d, e, weight);
}

/** @returns the result of a polynomial of the fifth degree. */
template<typename Type>
constexpr Type quinticFunction (Type a, Type b, Type c, Type d, Type e, Type f, Type weight) noexcept
{
    return (a * sursolid (weight))
         + quarticFunction (b, c, d, e, f, weight);
}

/** @returns the result of a polynomial of the sixth degree. */
template<typename Type>
constexpr Type sexticFunction (Type a, Type b, Type c, Type d, Type e, Type f, Type g, Type weight) noexcept
{
    return (a * zenzicube (weight))
         + quinticFunction (b, c, d, e, f, g, weight);
}
