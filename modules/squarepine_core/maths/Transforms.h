//==============================================================================
/** Sinc function */
inline double sinc (double x) noexcept      { return approximatelyEqual (x, 0.0) ? 1.0 : (std::sin (x) / x); }

/** Sinc function */
inline float sinc (float x) noexcept        { return x == 0.0f ? 1.0f : (std::sin (x) / x); }

/** Sinc function */
template<typename Type>
inline Type sinc (Type x) noexcept          { return (Type) sinc ((double) x); }

//==============================================================================
/** Normalised Sinc function */
template<typename Type>
inline double normalisedSinc (Type x) noexcept
{
    return sinc (static_cast<double> (x) * MathConstants<double>::pi);
}

/** Normalised Sinc function */
inline float normalisedSinc (float x) noexcept
{
    return sinc (x * MathConstants<float>::pi);
}
