/** This is an amalgamation of a variety of standard animation easing functions,
    alongside some convenient standard audio waveform functions.

    The said audio waveform functions are provided with
    a similar API in mind in that the easing functions,
    which are effectively complex interpolation functions,
    require a weight to be provided instead of a phase.
*/
namespace ease
{
    /** This namespace provides an assortment of the standard CSS easing functions.

        @see https://easings.net/
    */
    namespace cubic
    {
        /** */
        namespace in
        {
            /** @returns */
            [[nodiscard]] inline constexpr double quad (double weight) noexcept     { return square (weight); }
            /** @returns */
            [[nodiscard]] inline constexpr double cubic (double weight) noexcept    { return cube (weight); }
            /** @returns */
            [[nodiscard]] inline constexpr double quart (double weight) noexcept    { return biquadrate (weight); }
            /** @returns */
            [[nodiscard]] inline constexpr double quint (double weight) noexcept    { return sursolid (weight); }
            /** @returns */
            [[nodiscard]] inline constexpr double expo (double weight)              { return capproximatelyEqual (weight, 0.0) ? 0.0 : std::pow (2.0, 10.0 * weight - 10.0); }
            /** @returns */
            [[nodiscard]] inline double circ (double weight)                        { return 1.0 - std::sqrt (1.0 - square (weight)); }
            /** @returns */
            [[nodiscard]] inline double sine (double weight)                        { return 1.0 - std::cos ((weight * MathConstants<double>::pi) / 2.0); }
            /** @returns */
            [[nodiscard]] inline double back (double weight)                        { return 2.70158 * cube (weight) - 1.70158 * square (weight); }

            /** @returns a value that may be negative! */
            [[nodiscard]] inline double elastic (double weight)
            {
                if (capproximatelyEqual (weight, 0.0)) return 0.0;
                if (capproximatelyEqual (weight, 1.0)) return 1.0;

                constexpr auto c5 = MathConstants<double>::twoPi / 3.0;

                return -std::pow (2.0, 10.0 * weight - 10.0)
                     * std::sin ((weight * 10.0 - 10.75) * c5);
            }
        }

        /** */
        namespace out
        {
            /** @returns */
            [[nodiscard]] inline constexpr double quad (double weight) noexcept     { return 1.0 - square (1.0 - weight); }
            /** @returns */
            [[nodiscard]] inline constexpr double cubic (double weight) noexcept    { return 1.0 - cube (1.0 - weight); }
            /** @returns */
            [[nodiscard]] inline constexpr double quart (double weight) noexcept    { return 1.0 - biquadrate (1.0 - weight); }
            /** @returns */
            [[nodiscard]] inline constexpr double quint (double weight) noexcept    { return 1.0 - sursolid (1.0 - weight); }
            /** @returns */
            [[nodiscard]] inline double expo (double weight)                        { return capproximatelyEqual (weight, 1.0) ? 1.0 : 1.0 - std::pow (2.0, -10.0 * weight); }
            /** @returns */
            [[nodiscard]] inline double circ (double weight)                        { return std::sqrt (1.0 - square (weight - 1.0)); }
            /** @returns */
            [[nodiscard]] inline double sine (double weight)                        { return std::sin ((weight * MathConstants<double>::pi) / 2.0); }
            /** @returns */
            [[nodiscard]] inline constexpr double back (double weight) noexcept     { return 1.0 + 2.70158 * cube (weight - 1.0) + 1.70158 * square (weight - 1.0); }

            /** @returns a value that may be negative! */
            [[nodiscard]] inline double elastic (double weight)
            {
                if (capproximatelyEqual (weight, 0.0)) return 0.0;
                if (capproximatelyEqual (weight, 1.0)) return 1.0;

                constexpr auto c5 = MathConstants<double>::twoPi / 3.0;

                return 1.0
                     + (-std::pow (2.0, -10.0 * weight)
                        * std::sin ((weight * 10.0 - 10.75) * c5));
            }

            /** @returns */
            [[nodiscard]] inline constexpr double bounce (double weight) noexcept
            {
                constexpr auto n1 = 7.5625;
                constexpr auto d1 = 2.75;

                if (weight < (1.0 / d1))
                    return n1 * square (weight);
                else if (weight < (2.0 / d1))
                    return n1 * square (weight - 1.5 / d1) + 0.75;
                else if (weight < (2.5 / d1))
                    return n1 * square (weight - 2.25 / d1) + 0.9375;
                else
                    return n1 * square (weight - 2.625 / d1) + 0.984375;
            }
        }

        /** */
        namespace inOut
        {
            /** @returns */
            [[nodiscard]] inline double inOutPre (double pre, double weight, std::function<double (double)> func) noexcept
            {
                return weight < 0.5
                        ? pre * func (weight)
                        : 1.0 - func (-2.0 * weight + 2.0) / 2.0;
            }

            /** @returns */
            [[nodiscard]] inline double quad (double weight) noexcept   { return inOutPre (2.0, weight, square<double>); }
            /** @returns */
            [[nodiscard]] inline double cubic (double weight) noexcept  { return inOutPre (4.0, weight, cube<double>); }
            /** @returns */
            [[nodiscard]] inline double quart (double weight) noexcept  { return inOutPre (8.0, weight, biquadrate<double>); }
            /** @returns */
            [[nodiscard]] inline double quint (double weight) noexcept  { return inOutPre (16.0, weight, sursolid<double>); }

            /** @returns */
            [[nodiscard]] inline double expo (double weight)
            {
                if (capproximatelyEqual (weight, 0.0)) return 0.0;
                if (capproximatelyEqual (weight, 1.0)) return 1.0;

                return weight < 0.5
                        ? std::pow (2.0, 20.0 * weight - 10.0) / 2.0
                        : (2.0 - std::pow (2.0, -20.0 * weight + 10.0)) / 2.0;
            }

            /** @returns */
            [[nodiscard]] inline double circ (double weight)
            {
                return weight < 0.5
                        ? (1.0 - std::sqrt (1.0 - square (2.0 * weight))) / 2.0
                        : (1.0 + std::sqrt (1.0 - square (-2.0 * weight + 2.0))) / 2.0;
            }

            /** @returns */
            [[nodiscard]] inline double sine (double weight)
            {
                return -(std::cos (MathConstants<double>::pi * weight) - 1.0) / 2.0;
            }

            /** @returns */
            [[nodiscard]] inline constexpr double back (double weight) noexcept
            {
                constexpr auto c1 = 1.70158;
                constexpr auto c2 = c1 * 1.525;

                return weight < 0.5
                          ? (square (2.0 * weight)       * ((c2 + 1.0) * 2.0 * weight - c2)) / 2.0
                          : (square (2.0 * weight - 2.0) * ((c2 + 1.0) * (weight * 2.0 - 2.0) + c2) + 2.0) / 2.0;
            }

            /** @returns a value that may be negative! */
            [[nodiscard]] inline double elastic (double weight)
            {
                if (capproximatelyEqual (weight, 0.0)) return 0.0;
                if (capproximatelyEqual (weight, 1.0)) return 1.0;

                constexpr auto c5 = MathConstants<double>::twoPi / 4.5;

                return weight < 0.5
                        ? -(std::pow (2.0, 20.0 * weight - 10.0) * std::sin ((20.0 * weight - 11.125) * c5)) / 2.0
                        :  (std::pow (2.0, -20.0 * weight + 10.0) * std::sin ((20.0 * weight - 11.125) * c5)) / 2.0 + 1.0;
            }

            /** @returns */
            [[nodiscard]] inline constexpr double bounce (double weight) noexcept
            {
                 return weight < 0.5
                        ? (1.0 - ease::cubic::out::bounce (1.0 - 2.0 * weight)) / 2.0
                        : (1.0 + ease::cubic::out::bounce (2.0 * weight - 1.0)) / 2.0;
            }
        }

        namespace in
        {
            /** @returns */
            [[nodiscard]] inline constexpr double bounce (double weight) noexcept
            {
                return 1.0 - ease::cubic::out::bounce (1.0 - std::clamp (weight, 0.0, 1.0));
            }
        }
    }

    /** Contains a group of audio tone generation functions
        in the form of normalised/easing functions.

        You probably don't want to use these for audio purposes!
    */
    namespace audio
    {
        /** @returns */
        [[nodiscard]] inline double convertWeightToRads (double weight, double frequencyHz = 1.0)
        {
            return weight * frequencyHz * MathConstants<double>::twoPi;
        }

        /** @returns */
        [[nodiscard]] inline constexpr double linear (double weight) noexcept               { return weight; }
        /** @returns */
        [[nodiscard]] inline constexpr double smoothstepEase (double weight) noexcept       { return smoothstep (0.0, 1.0, weight); }
        /** @returns */
        [[nodiscard]] inline constexpr double smootherstepEase (double weight) noexcept     { return smootherstep (0.0, 1.0, weight); }
        /** @returns */
        [[nodiscard]] inline constexpr double sgnEase (double weight) noexcept              { return sgn (weight); }
        /** @returns */
        [[nodiscard]] inline double sinEase (double weight)                                 { return std::sin (convertWeightToRads (weight)); }
        /** @returns */
        [[nodiscard]] inline double cosEase (double weight)                                 { return std::cos (convertWeightToRads (weight)); }
        /** @returns */
        [[nodiscard]] inline double sincEase (double weight) noexcept                       { return sinc (convertWeightToRads (weight * 10.0)); }
        /** @returns */
        [[nodiscard]] inline constexpr double squareWave (double weight) noexcept           { return weight >= 0.5 ? 1.0 : 0.0; }

        /** @returns */
        [[nodiscard]] inline double sawtoothWave (double weight)
        {
            return std::acos (std::sin (convertWeightToRads (weight))) / 1.5708;
        }

        /** @returns */
        [[nodiscard]] inline double triangleWave (double weight)
        {
            constexpr auto frequencyHz = 1.0;
            constexpr auto fullPeriodTime = 1.0 / frequencyHz;

            const auto t = std::fmod (weight, fullPeriodTime) / fullPeriodTime;
            const auto value = 4.0 * t;

            if (t < 0.25)       return value;
            else if (t < 0.75)  return 2.0 - value;

            return value - 4.0;
        }
    }
}
