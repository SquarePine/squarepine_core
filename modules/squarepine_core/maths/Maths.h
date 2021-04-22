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
/** @returns a new AffineTransform, translated and rotated. */
inline AffineTransform createTranslationAndRotation (float x, float y, float angleRads) noexcept
{
    const auto cosRad = std::cos (angleRads);
    const auto sinRad = std::sin (angleRads);

    return AffineTransform (cosRad, -sinRad, x, sinRad, cosRad, y);
}

/** @returns a new AffineTransform, translated and scaled. */
inline AffineTransform createTranslationAndScale (float x, float y, float scaleX, float scaleY) noexcept
{
    return AffineTransform (scaleX, 0.0f, x, scaleY, 0.0f, y);
}

/** @returns a new AffineTransform, translated and scaled. */
inline AffineTransform createTranslationAndScale (float x, float y, float scale) noexcept
{
    return createTranslationAndScale (x, y, scale, scale);
}

//==============================================================================
/** @returns true if the given value number is odd. */
template<typename Type>
constexpr bool isOdd (Type value) noexcept      { return (value % 2) != 0; }

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

        return isOdd (static_cast<int64> (intPart));
    }
}

#endif // DOXYGEN

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
template<typename Type, typename IndexType = int>
constexpr Type calculateNextAverage (Type newValue, Type currentValue, IndexType numValues) noexcept
{
    return static_cast<IndexType> ((newValue + (numValues * currentValue))
                                 / (numValues + static_cast<IndexType> (1)));
}

//==============================================================================
/** Finds the maximum value and location of this in a buffer regardless of sign. */
template<typename FloatingPointType, typename IndexType>
inline void findAbsoluteMax (const FloatingPointType* samples, IndexType numSamples,
                             IndexType& maxSampleLocation, FloatingPointType& maxSampleValue) noexcept
{
    maxSampleValue = static_cast<FloatingPointType> (0);

    for (IndexType i = 0; i < numSamples; ++i)
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
template<typename FloatingPointType, typename IndexType>
inline void normalise (FloatingPointType* samples, IndexType numSamples) noexcept
{
    auto max = static_cast<FloatingPointType> (0);

    IndexType location;
    findAbsoluteMax (samples, numSamples, location, max);

    if (max != 0)
    {
        const auto oneOverMax = 1 / max;

        for (IndexType i = 0; i < numSamples; ++i)
            samples[i] *= oneOverMax;
    }
    else
    {
        zeromem (samples, numSamples * sizeof (float));
    }
}

//==============================================================================
/** Finds the autocorrelation of a set of given samples.

    This will cross-correlate inputSamples with itself and put the result in
    output samples. Note that this uses a shrinking integration window, assuming
    values outside of numSamples are 0. This leads to an exponentially decreasing
    autocorrelation function.
 */
template<typename FloatingPointType, typename IndexType>
inline void autocorrelate (const FloatingPointType* inputSamples, IndexType numSamples, FloatingPointType* outputSamples) noexcept
{
    for (IndexType i = 0; i < numSamples; i++)
    {
        auto sum = static_cast<FloatingPointType> (0);

        for (IndexType j = 0; j < numSamples - i; j++)
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
template<typename FloatingPointType, typename IndexType>
inline void sdfAutocorrelate (const FloatingPointType* inputSamples, IndexType numSamples, FloatingPointType* outputSamples) noexcept
{
    for (IndexType i = 0; i < numSamples; i++)
    {
        auto sum = static_cast<FloatingPointType> (0);

        for (IndexType j = 0; j < numSamples - i; j++)
            sum += square (inputSamples[j] - inputSamples[j + i]);

        outputSamples[i] = sum;
    }
}

/** Performs a first order differential on the set of given samples.

    This is the same as finding the difference between each sample and the previous.
    Note that outputSamples can point to the same location as inputSamples.
 */
template<typename FloatingPointType, typename IndexType>
inline void differentiate (const FloatingPointType* inputSamples, IndexType numSamples, FloatingPointType* outputSamples) noexcept
{
    auto lastSample = static_cast<FloatingPointType> (0);

    for (IndexType i = 0; i < numSamples; ++i)
    {
        const auto currentSample = inputSamples[i];
        outputSamples[i] = currentSample - lastSample;
        lastSample = currentSample;
    }
}

/** @returns the median of a set of samples assuming they are sorted. */
template<typename FloatingPointType, typename IndexType>
inline FloatingPointType findMedian (const FloatingPointType* samples, IndexType numSamples) noexcept
{
    if (isEven (numSamples % 2))
        return samples[numSamples / 2];

    const auto lowerIndex = (IndexType) std::round ((double) numSamples / 2.0);
    const auto lowerSample = samples[lowerIndex];
    const auto upperSample = samples[lowerIndex + 1];

    return (lowerSample + upperSample) / 2;
}

//==============================================================================
/** @returns the Mean for a set of values. */
template<typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type findMean (Iterator beginIter, Iterator endIter)
{
    using Type = typename std::iterator_traits<Iterator>::value_type;

    auto sum = static_cast<Type> (0);

    for (auto iter = beginIter; iter != endIter; ++iter)
        sum += *iter;

    return sum / (Type) std::distance (beginIter, endIter);
}

/** @returns the Mean for a set of values. */
template<typename Type>
inline Type findMean (const juce::Array<Type>& values)
{
    return findMean (std::cbegin (values), std::cend (values));
}

/** @returns the Mean for a set of values. */
template<typename Type>
inline Type findMean (const std::vector<Type>& values)
{
    return findMean (values.cbegin(), values.cend());
}

/** @returns the Mean for a set of values. */
template<typename Type, size_t arraySize>
inline Type findMean (const std::array<Type, arraySize>& values)
{
    return findMean (values.cbegin(), values.cend());
}

/** @returns the Mean for a set of values. */
template<typename Type, size_t arraySize>
inline Type findMean (const Type (&values)[arraySize])
{
    return findMean (std::cbegin (values), std::cend (values));
}

//==============================================================================
/** @returns the variance of a set of values. */
template<typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type findVariance (Iterator beginIter, Iterator endIter)
{
    const auto mean = findMean (beginIter, endIter);

    auto sum = static_cast<typename std::iterator_traits<Iterator>::value_type> (0);

    for (auto iter = beginIter; iter != endIter; ++iter)
        sum += square (*iter - mean);

    return sum / std::distance (beginIter, endIter);
}

/** @returns the sample standard deviation for a set of values. */
template<typename Type>
inline Type findVariance (const juce::Array<Type>& values)
{
    return findVariance (std::cbegin (values), std::cend (values));
}

/** @returns the sample standard deviation for a set of values. */
template<typename Type>
inline Type findVariance (const std::vector<Type>& values)
{
    return findVariance (values.cbegin(), values.cend());
}

/** @returns the sample standard deviation for a set of values. */
template<typename Type, size_t arraySize>
inline Type findVariance (const std::array<Type, arraySize>& values)
{
    return findVariance (values.cbegin(), values.cend());
}

/** @returns the sample standard deviation for a set of values. */
template<typename Type, size_t arraySize>
inline Type findVariance (const Type (&values)[arraySize])
{
    return findVariance (std::cbegin (values), std::cend (values));
}

//==============================================================================
/** @returns the corrected variance for a set of values suitable for a sample standard deviation.

    @note the N - 1 in the formula to correct for small data sets.
*/
template<typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type findCorrectedVariance (Iterator beginIter, Iterator endIter)
{
    const auto mean = findMean (beginIter, endIter);
    auto sum = static_cast<typename std::iterator_traits<Iterator>::value_type> (0);

    for (auto iter = beginIter; iter != endIter; ++iter)
        sum += square (*iter - mean);

    return sum / (std::distance (beginIter, endIter) - 1);
}

/** @returns the corrected variance for a set of values suitable for a sample standard deviation.

    @note the N - 1 in the formula to correct for small data sets.
*/
template<typename Type>
inline Type findCorrectedVariance (const juce::Array<Type>& values)
{
    return findCorrectedVariance (std::cbegin (values), std::cend (values));
}

/** @returns the corrected variance for a set of values suitable for a sample standard deviation.

    @note the N - 1 in the formula to correct for small data sets.
*/
template<typename Type>
inline Type findCorrectedVariance (const std::vector<Type>& values)
{
    return findCorrectedVariance (values.cbegin(), values.cend());
}

/** @returns the corrected variance for a set of values suitable for a sample standard deviation.

    @note the N - 1 in the formula to correct for small data sets.
*/
template<typename Type, size_t arraySize>
inline Type findCorrectedVariance (const std::array<Type, arraySize>& values)
{
    return findCorrectedVariance (values.cbegin(), values.cend());
}

/** @returns the corrected variance for a set of values suitable for a sample standard deviation.

    @note the N - 1 in the formula to correct for small data sets.
*/
template<typename Type, size_t arraySize>
inline Type findCorrectedVariance (const Type (&values)[arraySize])
{
    return findCorrectedVariance (std::cbegin (values), std::cend (values));
}

//==============================================================================
/** @returns the sample standard deviation for a set of values. */
template<typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type findStandardDeviation (Iterator beginIter, Iterator endIter)
{
    return std::sqrt (findCorrectedVariance (beginIter, endIter));
}

/** @returns the sample standard deviation for a set of values. */
template<typename Type>
inline Type findStandardDeviation (const juce::Array<Type>& values)
{
    return findStandardDeviation (std::cbegin (values), std::cend (values));
}

/** @returns the sample standard deviation for a set of values. */
template<typename Type>
inline Type findStandardDeviation (const std::vector<Type>& values)
{
    return findStandardDeviation (values.cbegin(), values.cend());
}

/** @returns the sample standard deviation for a set of values. */
template<typename Type, size_t arraySize>
inline Type findStandardDeviation (const std::array<Type, arraySize>& values)
{
    return findStandardDeviation (values.cbegin(), values.cend());
}

/** @returns the sample standard deviation for a set of values. */
template<typename Type, size_t arraySize>
inline Type findStandardDeviation (const Type (&values)[arraySize])
{
    return findStandardDeviation (std::cbegin (values), std::cend (values));
}

//==============================================================================
/** @returns the RMS for a set of values. */
template<typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type findRMS (Iterator beginIter, Iterator endIter)
{
    auto sum = static_cast<typename std::iterator_traits<Iterator>::value_type> (0);

    for (auto iter = beginIter; iter != endIter; ++iter)
        sum += square (*iter);

    return std::sqrt (sum / std::distance (beginIter, endIter));
}

/** @returns the RMS for a set of values. */
template<typename Type>
inline Type findRMS (const juce::Array<Type>& values)
{
    return findRMS (std::cbegin (values), std::cend (values));
}

/** @returns the RMS for a set of values. */
template<typename Type>
inline Type findRMS (const std::vector<Type>& values)
{
    return findRMS (values.cbegin(), values.cend());
}

/** @returns the RMS for a set of values. */
template<typename Type, size_t arraySize>
inline Type findRMS (const std::array<Type, arraySize>& values)
{
    return findRMS (values.cbegin(), values.cend());
}

/** @returns the RMS for a set of values. */
template<typename Type, size_t arraySize>
inline Type findRMS (const Type (&values)[arraySize])
{
    return findRMS (std::cbegin (values), std::cend (values));
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
/** Creates a unique hash based on the contents of the provided source file.

    @param source   The source file to hash.
    @param hash     The destination hash, which will be empty if anything failed.

    @returns ok if the file could be hashed.
*/
template<typename HasherType = juce::SHA256>
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

    constexpr auto oneMiB = 1 << 20;

    if (fis.getTotalLength() < (oneMiB * 2))
    {
        hash = HasherType (fis).toHexString();
    }
    else
    {
        BufferedInputStream bis (&fis, oneMiB, false);
        hash = HasherType (bis).toHexString();
    }

    return Result::ok();
}
