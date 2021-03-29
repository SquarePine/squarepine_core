/** An assortment of audio sample disortion functions. */
class DistortionFunctions final
{
public:
    //==============================================================================
    /**

        @param inputSample
        @param amount       Within a normalised range

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType simple (FloatType inputSample, FloatType amount) noexcept
    {
        constexpr auto one = (FloatType) 1;
        constexpr auto two = (FloatType) 2;

        const auto s = std::abs (inputSample);

        amount = jmax (one, std::abs (amount) * static_cast<FloatType> (75));

        return (inputSample * (s + amount))
               / (std::pow (inputSample, two) + ((amount - one) * s) + one);
    }

    /**

        @param inputSample
        @param amount

        @returns a distorted sample.
    */
    static float simple (float inputSample, float amount) noexcept
    {
        return (float) simple (static_cast<double> (inputSample),
                               static_cast<double> (amount));
    }

    /**

        @param buffer
        @param amount
    */
    template<typename FloatType>
    static void performSimple (AudioBuffer<FloatType>& buffer, FloatType amount) noexcept
    {
        performSingleParameter<FloatType, &simple> (buffer, amount);
    }

    //==============================================================================
    /**

        @param inputSample

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType hyperbolicTangentSoftClipping (FloatType inputSample) noexcept
    {
        return std::tanh (inputSample * (FloatType) 5);
    }

    //==============================================================================
    /**

        @param inputSample

        @returns a distorted sample.
    */
    static double sinusoidalSoftClipping (double inputSample) noexcept
    {
        if (std::abs (inputSample) > (2.0 / 3.0))
            return sgn (inputSample);

        inputSample = 3.0 * MathConstants<double>::pi * inputSample;
        return std::sin (inputSample / 4.0);
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType sinusoidalSoftClipping (FloatType inputSample) noexcept
    {
        return static_cast<FloatType> (sinusoidalSoftClipping ((double) inputSample));
    }

    //==============================================================================
    /**

        @param inputSample

        @returns a distorted sample.
    */
    static double exponential2SoftClipping (double inputSample) noexcept
    {
        if (std::abs (inputSample) > (2.0 / 3.0))
            return sgn (inputSample);

        const auto a = 3.0 * inputSample / 2.0;
        const auto v = std::exp2 (std::abs (a - sgn (inputSample)));
        return sgn (1.0 - v);
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType exponential2SoftClipping (FloatType inputSample) noexcept
    {
        return static_cast<FloatType> (exponential2SoftClipping ((double) inputSample));
    }

    //==============================================================================
    /** TSQ

        @param inputSample

        @returns a distorted sample.
    */
    static double twoStageQuadraticSoftClipping (double inputSample) noexcept
    {
        if (std::abs (inputSample) > (2.0 / 3.0))
            return sgn (inputSample);

        if (std::abs (inputSample) < (1.0 / 3.0))
            return inputSample * 2.0;

        const auto a = 3.0 - square (2.0 - std::abs (3.0 * inputSample));
        return sgn (a / 3.0);
    }

    /** TSQ

        @param inputSample

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType twoStageQuadraticSoftClipping (FloatType inputSample) noexcept
    {
        return static_cast<FloatType> (twoStageQuadraticSoftClipping ((double) inputSample));
    }

    //==============================================================================
    /**

        @param inputSample

        @returns a distorted sample.
    */
    static double cubicSoftClipping (double inputSample) noexcept
    {
        if (std::abs (inputSample) > (2.0 / 3.0))
            return sgn (inputSample);

        const auto a = 9.0 * inputSample / 4.0;
        const auto b = 27.0 * cube (inputSample) / 16.0;
        return a - b;
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType cubicSoftClipping (FloatType inputSample) noexcept
    {
        return static_cast<FloatType> (cubicSoftClipping ((double) inputSample));
    }

    //==============================================================================
    /**

        @param inputSample

        @returns a distorted sample.
    */
    static double reciprocalSoftClipping (double inputSample) noexcept
    {
        if (std::abs (inputSample) > (2.0 / 3.0))
            return sgn (inputSample);

        const auto a = 30.0 * std::abs (inputSample) + 1.0;
        return sgn (1.0 - (1.0 / a));
    }

    /**

        @param inputSample

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType reciprocalSoftClipping (FloatType inputSample) noexcept
    {
        return static_cast<FloatType> (cubicSoftClipping ((double) inputSample));
    }

    //==============================================================================
    /**

        @param inputSample
        @param threshold    Within a normalised range

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType foldBack (FloatType inputSample, FloatType threshold) noexcept
    {
        if (threshold <= FloatType (0))
            return FloatType (0);

        if (inputSample > threshold || inputSample < threshold)
            return std::abs (std::abs (std::fmod (inputSample - threshold, threshold * 4)) - threshold * 2) - threshold;

        return inputSample;
    }

    /**

        @param inputSample
        @param threshold

        @returns a distorted sample.
    */
    static float foldBack (float inputSample, float threshold) noexcept
    {
        return (float) foldBack (static_cast<double> (inputSample),
                                 static_cast<double> (threshold));
    }

    /**

        @param buffer
        @param threshold
    */
    template<typename FloatType>
    static void performFoldBack (AudioBuffer<FloatType>& buffer, FloatType threshold) noexcept
    {
        performSingleParameter<FloatType, &foldBack> (buffer, threshold);
    }

    //==============================================================================
    /**

        @param inputSample
        @param threshold

        @returns a distorted sample.
    */
    template<typename FloatType>
    static constexpr FloatType hardClipping (FloatType inputSample, FloatType threshold = FloatType (1)) noexcept
    {
        return std::clamp (inputSample, -threshold, threshold);
    }

    /**

        @param buffer
        @param threshold
    */
    template<typename FloatType>
    static void performHardClipping (AudioBuffer<FloatType>& buffer, float threshold) noexcept
    {
        performSingleParameter<FloatType, &hardClipping> (buffer, threshold);
    }

    //==============================================================================
    /**

        @param inputSample
        @param lowerThreshold
        @param upperThreshold

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType softClipping (FloatType inputSample,
                                   FloatType lowerThreshold,
                                   FloatType upperThreshold) noexcept
    {
        const bool isNegativeClipping = inputSample < -lowerThreshold;

        if (isNegativeClipping || inputSample > lowerThreshold)
        {
            inputSample = std::abs (inputSample);

            if (inputSample > upperThreshold)
            {
                inputSample = static_cast<FloatType> (1);
            }
            else
            {
                const FloatType m = inputSample * upperThreshold;
                inputSample = upperThreshold - std::pow (lowerThreshold - m, static_cast<FloatType> (2));
                inputSample /= upperThreshold;
            }

            if (isNegativeClipping)
                inputSample *= static_cast<FloatType> (-1);
        }

        return inputSample;
    }

    /**

        @param buffer
        @param threshold
    */
    template<typename FloatType>
    static void performSoftClipping (AudioBuffer<FloatType>& buffer,
                                     Range<FloatType> threshold) noexcept
    {
        performDualParameter<FloatType, &softClipping> (buffer, threshold.getStart(), threshold.getEnd());
    }

    /**

        @param buffer
    */
    template<typename FloatType>
    static void performDefaultSoftClipping (AudioBuffer<FloatType>& buffer) noexcept
    {
        performSoftClipping (buffer, Range<FloatType> (static_cast<FloatType> (1.0 / 3.0), static_cast<FloatType> (2.0 / 3.0)));
    }

    //==============================================================================
    /**

        @param inputSample

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType softClippingExp (FloatType inputSample) noexcept
    {
        if (inputSample > static_cast<FloatType> (0))
            return static_cast<FloatType> (1) - std::exp (-inputSample);

        return static_cast<FloatType> (-1) + std::exp (inputSample);
    }

    /**

        @param buffer
    */
    template<typename FloatType>
    static void performSoftClippingExp (AudioBuffer<FloatType>& buffer) noexcept
    {
        perform<FloatType, &softClippingExp> (buffer);
    }

    //==============================================================================
    /**

        @param inputSample

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType halfWaveRectification (FloatType inputSample) noexcept
    {
        return (std::abs (inputSample) + inputSample) / static_cast<FloatType> (2);
    }

    /**

        @param buffer
    */
    template<typename FloatType>
    static void performHalfWaveRectification (AudioBuffer<FloatType>& buffer) noexcept
    {
        perform<FloatType, &halfWaveRectification> (buffer);
    }

    //==============================================================================
    /**

        @param inputSample

        @returns a distorted sample.
    */
    template<typename FloatType>
    static FloatType fullWaveRectification (FloatType inputSample) noexcept
    {
        return std::abs (inputSample);
    }

    /**

        @param buffer
    */
    template<typename FloatType>
    static void performFullWaveRectification (AudioBuffer<FloatType>& buffer) noexcept
    {
        perform<FloatType, &fullWaveRectification> (buffer);
    }

private:
    //==============================================================================
    template<typename FloatType, FloatType (*function) (FloatType)>
    static void perform (AudioBuffer<FloatType>& buffer) noexcept
    {
        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();
        auto** chans = buffer.getArrayOfWritePointers();

        for (auto i = numChannels; --i >= 0;)
            for (auto f = numSamples; --f >= 0;)
                chans[i][f] = (*function) (chans[i][f]);
    }

    template<typename FloatType, FloatType (*function) (FloatType, FloatType)>
    static void performSingleParameter (AudioBuffer<FloatType>& buffer, FloatType value) noexcept
    {
        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();
        auto** chans = buffer.getArrayOfWritePointers();

        for (int i = numChannels; --i >= 0;)
            for (int f = numSamples; --f >= 0;)
                chans[i][f] = (*function) (chans[i][f], value);
    }

    template<typename FloatType, FloatType (*function) (FloatType, FloatType, FloatType)>
    static void performDualParameter (AudioBuffer<FloatType>& buffer, FloatType valueA, FloatType valueB) noexcept
    {
        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();
        auto** chans = buffer.getArrayOfWritePointers();

        for (auto i = numChannels; --i >= 0;)
            for (auto f = numSamples; --f >= 0;)
                chans[i][f] = (*function) (chans[i][f], valueA, valueB);
    }

    //==============================================================================
    SQUAREPINE_DECLARE_TOOL_CLASS (DistortionFunctions)
};
