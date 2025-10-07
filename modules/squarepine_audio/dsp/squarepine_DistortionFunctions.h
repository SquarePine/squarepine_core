/** An assortment of audio sample distortion functions. */
template<typename FloatType>
class DistortionFunctions final
{
public:
    //==============================================================================
    /**

        @param inputSample  -1 to 1
        @param drive        0 to 1

        @returns a distorted sample.
    */
    [[nodiscard]] static FloatType simple (FloatType inputSample, FloatType drive)
    {
        const auto s = cabs (inputSample);

        drive = std::max (one, cabs (drive) * static_cast<FloatType> (75));

        return (inputSample * (s + drive))
               / (std::pow (inputSample, two) + ((drive - one) * s) + one);
    }

    /**

        @param inputSample  -1 to 1
        @param drive        0 to 1
        @param range        0 to 1000

        @returns a distorted sample.
    */
    [[nodiscard]] static FloatType sigmoid (FloatType inputSample,
                                            FloatType drive = one,
                                            FloatType range = static_cast<FloatType> (100))
    {
        const auto v = inputSample
                       * std::clamp (drive, zero, one)
                       * std::clamp (range, zero, static_cast<FloatType> (1000));

        return (two / MathConstants<FloatType>::pi) * std::atan (v);
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static constexpr FloatType halfWaveRectification (FloatType inputSample)
    {
        return (cabs (inputSample) + inputSample) / two;
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    [[nodiscard]] static constexpr FloatType fullWaveRectification (FloatType inputSample)
    {
        return cabs (inputSample);
    }

    //==============================================================================
    /** */
    template <typename... Args>
    static void perform (juce::AudioBuffer<FloatType>& buffer,
                         auto function, Args&&... args)
    {
        jassert (function != nullptr);

        for (auto channel : AudioBufferView (buffer))
            for (auto& sample : channel)
                sample = function (sample, std::forward<Args> (args)...);
    }

private:
    //==============================================================================
    static inline constexpr auto zero   = static_cast<FloatType> (0);
    static inline constexpr auto one    = static_cast<FloatType> (1);
    static inline constexpr auto two    = static_cast<FloatType> (2);

    //==============================================================================
    SQUAREPINE_DECLARE_TOOL_CLASS (DistortionFunctions)
};
