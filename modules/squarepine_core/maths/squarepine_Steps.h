//==============================================================================
/** Rectangular step function */
constexpr double rect (double x) noexcept   { return (sgn (x) + 1.0) / 2.0; }

/** Rectangular step function */
constexpr float rect (float x) noexcept     { return (sgn (x) + 1.0f) / 2.0f; }

/** Rectangular step function */
template<typename Type>
constexpr Type rect (Type x) noexcept       { return (Type) rect ((double) x); }

//==============================================================================
/** Heaviside function */
constexpr double heaviside (double x) noexcept
{
    if (x < 0.0)        return 0.0;
    else if (x > 0.0)   return 1.0;
    else                return 0.5;
}

/** Heaviside function */
constexpr float heaviside (float x) noexcept
{
    if (x < 0.0f)       return 0.0f;
    else if (x > 0.0f)  return 1.0f;
    else                return 0.5f;
}

/** Heaviside function */
template<typename Type>
constexpr Type heaviside (Type x) noexcept  { return (Type) rect ((double) x); }
