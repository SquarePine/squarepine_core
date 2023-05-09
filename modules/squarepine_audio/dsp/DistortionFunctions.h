/** An assortment of audio sample distortion functions. */
template<typename FloatType>
class DistortionFunctions final
{
public:
    //==============================================================================
    /**

        @param inputSample
        @param amount       Within a normalised range

        @returns a distorted sample.
    */
    [[nodiscard]] static FloatType simple (FloatType inputSample, FloatType drive) noexcept
    {
        const auto s = cabs (inputSample);

        drive = jmax (one, cabs (drive) * static_cast<FloatType> (75));

        return (inputSample * (s + drive))
               / (std::pow (inputSample, two) + ((drive - one) * s) + one);
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static FloatType hyperbolicTangentSoftClipping (FloatType inputSample) noexcept
    {
        return std::tanh (inputSample * static_cast<FloatType> (5));
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static FloatType sinusoidalSoftClipping (FloatType inputSample) noexcept
    {
        if (cabs (inputSample) > twoThirds)
            return sgn (inputSample);

        inputSample = three * MathConstants<FloatType>::pi * inputSample;
        return std::sin (inputSample / four);
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static FloatType exponential2SoftClipping (FloatType inputSample) noexcept
    {
        if (cabs (inputSample) > twoThirds)
            return sgn (inputSample);

        const auto a = three * inputSample / two;
        const auto v = std::exp2 (cabs (a - sgn (inputSample)));
        return sgn (one - v);
    }

    /** TSQ

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static constexpr FloatType twoStageQuadraticSoftClipping (FloatType inputSample) noexcept
    {
        if (cabs (inputSample) > twoThirds)
            return sgn (inputSample);

        if (cabs (inputSample) < oneThird)
            return inputSample * two;

        const auto a = three - square (two - cabs (three * inputSample));
        return sgn (a / three);
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static constexpr FloatType cubicSoftClipping (FloatType inputSample) noexcept
    {
        if (cabs (inputSample) > twoThirds)
            return sgn (inputSample);

        const auto a = static_cast<FloatType> (9) * inputSample / four;
        const auto b = static_cast<FloatType> (27) * cube (inputSample) / static_cast<FloatType> (16);
        return a - b;
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static constexpr FloatType reciprocalSoftClipping (FloatType inputSample) noexcept
    {
        if (cabs (inputSample) > twoThirds)
            return sgn (inputSample);

        const auto a = static_cast<FloatType> (30) * cabs (inputSample) + one;
        return sgn (one - (one / a));
    }

    /**

        @param inputSample
        @param threshold    Within a normalised range

        @returns a distorted sample.
    */
    [[nodiscard]] static FloatType foldBack (FloatType inputSample, FloatType threshold) noexcept
    {
        if (threshold <= zero)
            return zero;

        if (inputSample > threshold || inputSample < threshold)
            return cabs (cabs (std::fmod (inputSample - threshold, threshold * four)) - threshold * two) - threshold;

        return inputSample;
    }

    /**

        @param inputSample
        @param threshold

        @returns a distorted sample.
    */
    [[nodiscard]] static constexpr FloatType hardClipping (FloatType inputSample,
                                                           FloatType threshold = one) noexcept
    {
        return std::clamp (inputSample, -threshold, threshold);
    }

    /**

        @param inputSample
        @param lowerThreshold
        @param upperThreshold

        @returns a distorted sample.
    */
    [[nodiscard]] static FloatType softClipping (FloatType inputSample,
                                                 FloatType lowerThreshold,
                                                 FloatType upperThreshold) noexcept
    {
        const bool isNegativeClipping = inputSample < -lowerThreshold;

        if (isNegativeClipping || inputSample > lowerThreshold)
        {
            inputSample = cabs (inputSample);

            if (inputSample > upperThreshold)
            {
                inputSample = one;
            }
            else
            {
                const auto m = inputSample * upperThreshold;
                inputSample = upperThreshold - std::pow (lowerThreshold - m, two);
                inputSample /= upperThreshold;
            }

            if (isNegativeClipping)
                inputSample *= -one;
        }

        return inputSample;
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static FloatType softClippingExp (FloatType inputSample) noexcept
    {
        if (inputSample > zero)
            return one - std::exp (-inputSample);

        return -one + std::exp (inputSample);
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static constexpr FloatType halfWaveRectification (FloatType inputSample) noexcept
    {
        return (cabs (inputSample) + inputSample) / two;
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static constexpr FloatType fullWaveRectification (FloatType inputSample) noexcept
    {
        return cabs (inputSample);
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static constexpr FloatType tubeSaturation (FloatType inputSample, FloatType drive)
    {
        const auto output = twoPi * std::atan (inputSample * drive);
        // mix dry and wet signals
        return (0.5 * inputSample) + (0.5 * output);
    }

    //==============================================================================
    static void perform (juce::AudioBuffer<FloatType>& buffer,
                         std::function<FloatType (FloatType)> function) noexcept
    {
        jassert (function != nullptr);

        for (auto channel : AudioBufferView (buffer))
            for (auto& sample : channel)
                sample = function (sample);
    }

    static void perform (juce::AudioBuffer<FloatType>& buffer,
                         FloatType value,
                         std::function<FloatType (FloatType, FloatType)> function) noexcept
    {
        jassert (function != nullptr);

        for (auto channel : AudioBufferView (buffer))
            for (auto& sample : channel)
                sample = function (sample, value);
    }

    static void perform (juce::AudioBuffer<FloatType>& buffer,
                         FloatType valueA, FloatType valueB,
                         std::function<FloatType (FloatType, FloatType, FloatType)> function) noexcept
    {
        jassert (function != nullptr);

        for (auto channel : AudioBufferView (buffer))
            for (auto& sample : channel)
                sample = function (sample, valueA, valueB);
    }

private:
    //==============================================================================
    static inline constexpr auto zero       = static_cast<FloatType> (0);
    static inline constexpr auto one        = static_cast<FloatType> (1);
    static inline constexpr auto two        = static_cast<FloatType> (2);
    static inline constexpr auto three      = static_cast<FloatType> (3);
    static inline constexpr auto four       = static_cast<FloatType> (4);

    static inline constexpr auto oneThird   = static_cast<FloatType> (1.0 / 3.0);
    static inline constexpr auto twoThirds  = static_cast<FloatType> (2.0 / 3.0);

    static inline constexpr auto pi         = MathConstants<FloatType>::pi;
    static inline constexpr auto twoPi      = MathConstants<FloatType>::twoPi;

    //==============================================================================
    SQUAREPINE_DECLARE_TOOL_CLASS (DistortionFunctions)
};
