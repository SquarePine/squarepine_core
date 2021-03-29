//==============================================================================
/** @returns true if a value is within the provided range.

    When making this comparison, the start value is considered to be inclusive,
    and the end of the range exclusive.
*/
template<typename Type>
inline bool isValueBetween (Type value, const Range<Type>& range) noexcept
{
    return range.contains (value);
}

/** @returns true if a value is within the provided range.

    When making this comparison, the start value is considered to be inclusive,
    and the end of the range exclusive.
*/
template<typename Type>
inline bool isValueBetween (Type value, Type start, Type end) noexcept
{
    return isValueBetween (value, Range<Type> (start, end));
}

//==============================================================================
/** Creates a new AffineTransform, translated and rotated */
inline AffineTransform createTranslationAndRotation (float x, float y, float angleRads) noexcept
{
    const auto cosRad = std::cos (angleRads);
    const auto sinRad = std::sin (angleRads);

    return AffineTransform (cosRad, -sinRad, x, sinRad, cosRad, y);
}

/** Creates a new AffineTransform, translated and scaled */
inline AffineTransform createTranslationAndScale (float x, float y, float scaleX, float scaleY) noexcept
{
    return AffineTransform (scaleX, 0.0f, x, scaleY, 0.0f, y);
}

/** Creates a new AffineTransform, translated and scaled */
inline AffineTransform createTranslationAndScale (float x, float y, float scale) noexcept
{
    return createTranslationAndScale (x, y, scale, scale);
}

//==============================================================================
/** @returns true if the given value number is odd. */
template<typename Type>
constexpr bool isOdd (Type value) noexcept
{
    return (value % 2) != 0;
}

#if ! DOXYGEN

namespace detail
{
    /** @returns true if the given value number is odd. */
    template<typename FloatType>
    inline bool isFloatOdd (FloatType value) noexcept
    {
        constexpr auto zero = static_cast<FloatType> (0);

        auto intPart = zero;
        const auto fractionalPart = std::modf (value, &intPart);
        if (approximatelyEqual (fractionalPart, zero))
            return false;

        return isOdd (static_cast<int64_t> (intPart));
    }
}

#endif

/** @returns true if the given value number is odd. */
inline bool isOdd (float value) noexcept        { return detail::isFloatOdd (value); }
/** @returns true if the given value number is odd. */
inline bool isOdd (double value) noexcept       { return detail::isFloatOdd (value); }
/** @returns true if the given value number is odd. */
inline bool isOdd (long double value) noexcept  { return detail::isFloatOdd (value); }

//==============================================================================
/** @returns true if the given value is even. */
template<typename Type>
constexpr bool isEven (Type value) noexcept     { return ! isOdd (value); }
/** @returns true if the given value is even. */
inline bool isEven (float value) noexcept       { return ! isOdd (value); }
/** @returns true if the given value is even. */
inline bool isEven (double value) noexcept      { return ! isOdd (value); }
/** @returns true if the given value is even. */
inline bool isEven (long double value) noexcept { return ! isOdd (value); }

//==============================================================================
/** @returns the cube of its argument. */
template<typename NumericType>
constexpr NumericType cube (NumericType n) noexcept { return n * n * n; }

/** @returns the biquadrate of its argument. */
template<typename NumericType>
constexpr NumericType biquadrate (NumericType n) noexcept { return square (n) * square (n); }

/** @returns the sursolid of its argument. */
template<typename NumericType>
constexpr NumericType sursolid  (NumericType n) noexcept { return n * biquadrate (n); }

/** @returns the zenzicube of its argument. */
template<typename NumericType>
constexpr NumericType zenzicube (NumericType n) noexcept { return cube (n) * cube (n); }

//==============================================================================
/** @returns base to the power of exponent as an integral.

    @param base
    @param exponent
*/
template<typename IntegralType>
constexpr IntegralType ipow (IntegralType base, IntegralType exponent) noexcept
{
    return exponent == IntegralType (0)
        ? 1
        : base * ipow (base, exponent - 1);
}

/** @returns the smallest power-of-two which is equal to or greater than the given integer. */
inline int64 nextPowerOfTwo (int64 n) noexcept
{
    int64 power = 1;
    while (power < n)
        power *= 2;

    return power;
}

/** @returns the largest power-of-two which is equal to or less than the given IntegralType.

    e.g. the previous power of two for 566 is 512.

    @param x
*/
template<typename IntegralType>
inline IntegralType previousPowerOfTwo (IntegralType x) noexcept
{
    constexpr auto m = (IntegralType) sizeof (IntegralType) * (IntegralType) CHAR_BIT;

    for (IntegralType i = 0; ipow ((IntegralType) 2, i) < m; ++i)
        x |= x >> ipow ((IntegralType) 2, i);

    return x - (x >> 1);
}

//==============================================================================
/** Signum function */
constexpr double sgn (double x) noexcept
{
    return x > 0.0
        ? 1.0
        : (x < 0.0 ? -1.0 : 0.0);
}

/** Signum function */
constexpr float sgn (float x) noexcept
{
    return x > 0.0f
        ? 1.0f
        : (x < 0.0f ? -1.0f : 0.0f);
}

/** Signum function */
template<typename Type>
constexpr Type sgn (Type x) noexcept
{
    return x > Type (0)
        ? Type (1)
        : (x < Type (0) ? Type (-1) : Type (0));
}

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
constexpr Type heaviside (Type x) noexcept { return (Type) rect ((double) x); }

//==============================================================================
/** Cas function */
inline double cas (double x) noexcept    { return std::sin (x) + std::cos (x); }

/** Cas function */
inline float cas (float x) noexcept      { return std::sin (x) + std::cos (x); }

/** Cas function */
template<typename Type>
inline Type cas (Type x) noexcept        { return (Type) cas ((double) x); }

//==============================================================================
/** Cotangent function */
inline double cot (double x) noexcept    { return std::cos (x) / std::sin (x); }

/** Cotangent function */
inline float cot (float x) noexcept      { return std::cos (x) / std::sin (x); }

/** Cotangent function */
template<typename Type>
inline Type cot (Type x) noexcept        { return (Type) cot ((double) x); }

//==============================================================================
/** Cosecant function */
inline double csc (double x) noexcept    { return 1.0 / std::sin (x); }

/** Cosecant function */
inline float csc (float x) noexcept      { return 1.0f / std::sin (x); }

/** Cosecant function */
template<typename Type>
inline Type csc (Type x) noexcept        { return (Type) csc ((double) x); }

//==============================================================================
/** Secant function */
inline double sec (double x) noexcept    { return 1.0 / std::cos (x); }

/** Secant function */
inline float sec (float x) noexcept      { return 1.0f / std::cos (x); }

/** Secant function */
template<typename Type>
inline Type sec (Type x) noexcept        { return (Type) sec ((double) x); }

//==============================================================================
/** Versine function */
inline double versin (double x) noexcept { return 1.0 - std::cos (x); }

/** Versine function */
inline float versin (float x) noexcept   { return 1.0f - std::cos (x); }

/** Versine function */
template<typename Type>
inline Type versin (Type x) noexcept     { return (Type) versin ((double) x); }

//==============================================================================
/** Vercosine function */
inline double vercosin (double x) noexcept   { return 1.0 + std::cos (x); }

/** Vercosine function */
inline float vercosin (float x) noexcept     { return 1.0f + std::cos (x); }

/** Vercosine function */
template<typename Type>
inline Type vercosin (Type x) noexcept       { return (Type) vercosin ((double) x); }

//==============================================================================
/** Coversine function */
inline double coversin (double x) noexcept   { return 1.0 + std::cos (x); }

/** Coversine function */
inline float coversin (float x) noexcept     { return 1.0f + std::cos (x); }

/** Coversine function */
template<typename Type>
inline Type coversin (Type x) noexcept       { return (Type) vercosin ((double) x); }

//==============================================================================
/** Exsecant function */
inline double exsec (double x) noexcept  { return sec (x) - 1.0; }

/** Exsecant function */
inline float exsec (float x) noexcept    { return sec (x) - 1.0f; }

/** Exsecant function */
template<typename Type>
inline Type exsec (Type x) noexcept      { return (Type) exsec ((double) x); }

//==============================================================================
/** Excosecant function */
inline double excsc (double x) noexcept  { return csc (x) - 1.0; }

/** Excosecant function */
inline float excsc (float x) noexcept    { return csc (x) - 1.0f; }

/** Excosecant function */
template<typename Type>
inline Type excsc (Type x) noexcept      { return (Type) excsc ((double) x); }

//==============================================================================
/** Sinc function */
inline double sinc (double x) noexcept   { return approximatelyEqual (x, 0.0) ? 1.0 : (std::sin (x) / x); }

/** Sinc function */
inline float sinc (float x) noexcept     { return x == 0.0f ? 1.0f : (std::sin (x) / x); }

/** Sinc function */
template<typename Type>
inline Type sinc (Type x) noexcept       { return (Type) sinc ((double) x); }

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

//==============================================================================
/** */
constexpr double quadraticFunction (double a, double b, double c, double weight) noexcept
{
    return (a * square (weight))
         + (b * weight)
         + c;
}

/** */
constexpr double cubicFunction (double a, double b, double c, double d, double weight) noexcept
{
    return (a * cube (weight))
         + quadraticFunction (b, c, d, weight);
}

/** */
constexpr double quarticFunction (double a, double b, double c, double d, double e, double weight) noexcept
{
    return (a * biquadrate (weight))
         + cubicFunction (b, c, d, e, weight);
}

//==============================================================================
constexpr float lerp (float a, float b, float weight) noexcept
{
    return (a * (1.0f - weight)) + (b * weight);
}

constexpr double lerp (double a, double b, double weight) noexcept
{
    return (a * (1.0 - weight)) + (b * weight);
}

template<typename Type>
constexpr Type lerp (Type a, Type b, Type weight) noexcept
{
    return (a * (static_cast<Type> (1) - weight)) + (b * weight);
}

//==============================================================================
inline float smoothstep (float a, float b, float weight) noexcept
{
    const auto w = std::clamp ((weight - a) / (b - a), 0.0f, 1.0f);
    return square (w) * (3.0f - (2.0f * w));
}

inline double smoothstep (double a, double b, double weight) noexcept
{
    const auto w = std::clamp ((weight - a) / (b - a), 0.0, 1.0);
    return square (w) * (3.0 - (2.0 * w));
}

template<typename Type>
inline Type smoothstep (Type a, Type b, Type weight) noexcept
{
    return (Type) smoothstep ((double) a, (double) b, (double) weight);
}

//==============================================================================
inline float smootherstep (float a, float b, float weight) noexcept
{
    const auto w = std::clamp ((weight - a) / (b - a), 0.0f, 1.0f);
    return cube (w) * (w * (w * 6.0f - 15.0f) + 10.0f);
}

inline double smootherstep (double a, double b, double weight) noexcept
{
    const auto w = std::clamp ((weight - a) / (b - a), 0.0, 1.0);
    return cube (w) * (w * (w * 6.0 - 15.0) + 10.0);
}

template<typename Type>
inline Type smootherstep (Type a, Type b, Type weight) noexcept
{
    return (Type) smootherstep ((double) a, (double) b, (double) weight);
}

//==============================================================================
inline float inverseSmoothstep (float x) noexcept
{
    return 0.5f - std::sin (std::asin (1.0f - 2.0f * x) / 3.0f);
}

inline double inverseSmoothstep (double x) noexcept
{
    return 0.5 - std::sin (std::asin (1.0 - 2.0 * x) / 3.0);
}

//==============================================================================
template<typename Type, typename IndexType = int>
constexpr Type calculateNextAverage (Type newValue, Type currentValue, IndexType numValues) noexcept
{
    return static_cast<IndexType> ((newValue + (numValues * currentValue))
                                 / (numValues + static_cast<IndexType> (1)));
}

//==============================================================================
template<typename Type>
constexpr Type cubicInterpolation (Type a, Type b, Type c, Type d, Type weight) noexcept
{
    return d
         + (c * weight)
         + (b * square (weight))
         + (a * cube (weight));
}

template<typename Type>
inline Type catmullRomInterpolation (Type x, Type y, Type z, Type w, Type weight) noexcept
{
    constexpr auto half = static_cast<Type> (0.5);
    constexpr auto opf  = static_cast<Type> (1.5);
    constexpr auto tpo  = static_cast<Type> (2.0);
    constexpr auto tpf  = static_cast<Type> (2.5);

    const auto negHalfX = x * -half;
    const auto halfW = w * half;

    const auto a0 = y;
    const auto a1 = negHalfX + (y * half);
    const auto a2 = x - (y * tpf) + (z * tpo) - halfW;
    const auto a3 = negHalfX + (y * opf) - (z * opf) + halfW;

    return cubicInterpolation (a0, a1, a2, a3, weight);
}

//==============================================================================
/** Finds the maximum value and location of this in a buffer regardless of sign. */
template<typename FloatingPointType, typename IndexingType>
inline void findAbsoluteMax (const FloatingPointType* samples, IndexingType numSamples,
                             IndexingType& maxSampleLocation, FloatingPointType& maxSampleValue) noexcept
{
    maxSampleValue = static_cast<FloatingPointType> (0);

    for (IndexingType i = 0; i < numSamples; ++i)
    {
        const auto absoluteSample = std::abs (samples[i]);

        if (absoluteSample > maxSampleValue)
        {
            maxSampleValue = absoluteSample;
            maxSampleLocation = i;
        }
    }
}

/** Normalises a set of samples to the absolute maximum contained within the buffer. */
template<typename FloatingPointType, typename IndexingType>
inline void normalise (FloatingPointType* samples, IndexingType numSamples) noexcept
{
    auto max = static_cast<FloatingPointType> (0);

    IndexingType location;
    findAbsoluteMax (samples, numSamples, location, max);

    if (max != 0)
    {
        const auto oneOverMax = 1 / max;

        for (IndexingType i = 0; i < numSamples; ++i)
            samples[i] *= oneOverMax;
    }
    else
    {
        zeromem (samples, numSamples * sizeof (float));
    }
}

/** Squares all the values in an array. */
template<typename FloatingPointType, typename IndexingType>
inline void squareArray (FloatingPointType* samples, IndexingType numSamples)
{
    for (IndexingType i = 0; i < numSamples; ++i)
        samples[i] = square (samples[i]);
}

/** Finds the autocorrelation of a set of given samples.

    This will cross-correlate inputSamples with itself and put the result in
    output samples. Note that this uses a shrinking integration window, assuming
    values outside of numSamples are 0. This leads to an exponentially decreasing
    autocorrelation function.
 */
template<typename FloatingPointType, typename IndexingType>
inline void autocorrelate (const FloatingPointType* inputSamples, IndexingType numSamples, FloatingPointType* outputSamples) noexcept
{
    for (IndexingType i = 0; i < numSamples; i++)
    {
        auto sum = static_cast<FloatingPointType> (0);

        for (IndexingType j = 0; j < numSamples - i; j++)
            sum += inputSamples[j] * inputSamples[j + i];

        outputSamples[i] = sum * (static_cast<FloatingPointType> (1) / numSamples);
    }
}

/** Finds the autocorrelation of a set of given samples using a
    square-difference function.

    This will cross-correlate inputSamples with itself and put the result in
    output samples. Note that this uses a shrinking integration window, assuming
    values outside of numSamples are 0. This leads to an exponentially decreasing
    autocorrelation function.
 */
template<typename FloatingPointType, typename IndexingType>
inline void sdfAutocorrelate (const FloatingPointType* inputSamples, IndexingType numSamples, FloatingPointType* outputSamples) noexcept
{
    for (IndexingType i = 0; i < numSamples; i++)
    {
        auto sum = static_cast<FloatingPointType> (0);

        for (IndexingType j = 0; j < numSamples - i; j++)
            sum += square (inputSamples[j] - inputSamples[j + i]);

        outputSamples[i] = sum;
    }
}

/** Performs a first order differential on the set of given samples.

    This is the same as finding the difference between each sample and the previous.
    Note that outputSamples can point to the same location as inputSamples.
 */
template<typename FloatingPointType, typename IndexingType>
inline void differentiate (const FloatingPointType* inputSamples, IndexingType numSamples, FloatingPointType* outputSamples) noexcept
{
    auto lastSample = static_cast<FloatingPointType> (0);

    for (IndexingType i = 0; i < numSamples; ++i)
    {
        const auto currentSample = inputSamples[i];
        outputSamples[i] = currentSample - lastSample;
        lastSample = currentSample;
    }
}

/** @returns the mean of a set of samples. */
template<typename FloatingPointType, typename IndexingType>
inline FloatingPointType findMean (const FloatingPointType* samples, IndexingType numSamples) noexcept
{
    auto sum = static_cast<FloatingPointType> (0);

    for (IndexingType i = 0; i < numSamples; ++i)
        sum += samples[i];

    return sum / numSamples;
}

/** @returns the median of a set of samples assuming they are sorted. */
template<typename FloatingPointType, typename IndexingType>
inline FloatingPointType findMedian (const FloatingPointType* samples, IndexingType numSamples) noexcept
{
    if (isEven (numSamples % 2))
        return samples[numSamples / 2];

    const auto lowerIndex = (IndexingType) roundToInt ((double) numSamples / 2.0);
    const auto lowerSample = samples[lowerIndex];
    const auto upperSample = samples[lowerIndex + 1];

    return (lowerSample + upperSample) / 2;
}

/** @returns the variance of a set of samples. */
template<typename FloatingPointType, typename IndexingType>
inline FloatingPointType findVariance (const FloatingPointType* samples, IndexingType numSamples) noexcept
{
    const auto mean = findMean (samples, numSamples);

    auto sum = static_cast<FloatingPointType> (0);

    for (IndexingType i = 0; i < numSamples; ++i)
        sum += square (samples[i] - mean);

    return sum / numSamples;
}

/** @returns the corrected variance for a set of samples suitable for a sample standard deviation.

    @note the N - 1 in the formula to correct for small data sets.
 */
template<typename FloatingPointType, typename IndexingType>
inline FloatingPointType findCorrectedVariance (const FloatingPointType* samples, IndexingType numSamples) noexcept
{
    const auto mean = findMean (samples, numSamples);

    auto sum = static_cast<FloatingPointType> (0);

    for (IndexingType i = 0; i < numSamples; ++i)
        sum += square (samples[i] - mean);

    return sum / (numSamples - 1);
}

/** @returns the sample standard deviation for a set of samples. */
template<typename FloatingPointType, typename IndexingType>
inline FloatingPointType findStandardDeviation (const FloatingPointType* samples, IndexingType numSamples) noexcept
{
    return std::sqrt (findCorrectedVariance (samples, numSamples));
}

/** @returns the RMS for a set of samples. */
template<typename FloatingPointType, typename IndexingType>
inline FloatingPointType findRMS (const FloatingPointType* samples, IndexingType numSamples) noexcept
{
    auto sum = static_cast<FloatingPointType> (0);

    for (IndexingType i = 0; i < numSamples; ++i)
        sum += square (*samples++);

    return std::sqrt (sum / static_cast<FloatingPointType> (numSamples));
}

//============================================================================
/** @returns a pitch ratio converted from a number of semitones. */
inline float semitonesToPitchRatio (float numSemitones) noexcept    { return std::pow (2.0f, numSemitones / 12.0f); }
/** @returns a pitch ratio converted from a number of semitones. */
inline double semitonesToPitchRatio (double numSemitones) noexcept  { return std::pow (2.0, numSemitones / 12.0); }
/** @returns a pitch ratio converted from a number of semitones. */
template<typename Type>
inline double semitonesToPitchRatio (Type numSemitones) noexcept    { return semitonesToPitchRatio (static_cast<double> (numSemitones)); }

/** @returns a number semitones converted from a pitch ratio. */
inline float pitchRatioToSemitones (float pitchRatio) noexcept      { return 12.0f * std::log2 (pitchRatio); }
/** @returns a number semitones converted from a pitch ratio. */
inline double pitchRatioToSemitones (double pitchRatio) noexcept    { return 12.0 * std::log2 (pitchRatio); }
/** @returns a number semitones converted from a pitch ratio. */
template<typename Type>
inline double pitchRatioToSemitones (Type pitchRatio) noexcept      { return pitchRatioToSemitones (static_cast<double> (pitchRatio)); }

//============================================================================
/** @returns a MIDI note converted from a frequency. */
inline int frequencyToMIDINote (double frequency) noexcept
{
    const auto pitchRatio = jmax (0.0, frequency / 440.0);
    return roundToInt (69.0 + pitchRatioToSemitones (pitchRatio));
}

/** @returns a frequency converted from a MIDI note. */
inline double midiNoteToFrequency (int midiNoteNumber) noexcept
{
    const auto semitones = jmax (0, midiNoteNumber);
    return 440.0 * semitonesToPitchRatio (static_cast<double> (semitones) - 69.0);
}

//============================================================================
/** Creates a unique SHA-256 hash based on the contents of the provided source file.

    @param source   The source file to hash.
    @param hash     The destination hash, which will be empty if anything failed.

    @returns ok if the file could be hashed.
*/
inline Result createUniqueFileHash (const File& source, String& hash)
{
    hash.clear();

    if (! source.existsAsFile())
    {
        jassertfalse;
        return Result::fail (TRANS ("File does not exist."));
    }

    FileInputStream fis (source);
    if (fis.failedToOpen())
        return fis.getStatus();

    const auto s = Time::getCurrentTime();
    hash = SHA256 (fis).toHexString();
    const auto e = Time::getCurrentTime();

    auto millis = (e - s).inMilliseconds();
    Logger::writeToLog (String (millis));

    return Result::ok();
}
