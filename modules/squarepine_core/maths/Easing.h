//==============================================================================
namespace ease
{
    namespace cubic
    {
        namespace in
        {
            constexpr double quad (double weight) noexcept  { return square (weight); }
            constexpr double cubic (double weight) noexcept { return cube (weight); }
            constexpr double quart (double weight) noexcept { return biquadrate (weight); }
            constexpr double quint (double weight) noexcept { return sursolid (weight); }
            inline double expo (double weight)              { return approximatelyEqual (weight, 0.0) ? 0.0 : std::pow (2.0, 10.0 * weight - 10.0); }
            inline double circ (double weight)              { return 1.0 - std::sqrt (1.0 - square (weight)); }
            inline double sine (double weight)              { return 1.0 - std::cos ((weight * MathConstants<double>::pi) / 2.0); }
            inline double back (double weight)              { return 2.70158 * cube (weight) - 1.70158 * square (weight); }

            /** @returns a value that may be negative! */
            inline double elastic (double weight)
            {
                if (approximatelyEqual (weight, 0.0))   return 0.0;
                if (approximatelyEqual (weight, 1.0))   return 1.0;

                constexpr auto c5 = MathConstants<double>::twoPi / 3.0;

                return -std::pow (2.0, 10.0 * weight - 10.0)
                     * std::sin ((weight * 10.0 - 10.75) * c5);
            }
        }

        namespace out
        {
            constexpr double quad (double weight) noexcept  { return 1.0 - square (1.0 - weight); }
            constexpr double cubic (double weight)          { return 1.0 - cube (1.0 - weight); }
            constexpr double quart (double weight)          { return 1.0 - biquadrate (1.0 - weight); }
            constexpr double quint (double weight)          { return 1.0 - sursolid (1.0 - weight); }
            inline double expo (double weight)              { return approximatelyEqual (weight, 1.0) ? 1.0 : 1.0 - std::pow (2.0, -10.0 * weight); }
            inline double circ (double weight)              { return std::sqrt (1.0 - square (weight - 1.0)); }
            inline double sine (double weight)              { return std::sin ((weight * MathConstants<double>::pi) / 2.0); }
            constexpr double back (double weight) noexcept  { return 1.0 + 2.70158 * cube (weight - 1.0) + 1.70158 * square (weight - 1.0); }

            /** @returns a value that may be negative! */
            inline double elastic (double weight)
            {
                if (approximatelyEqual (weight, 0.0))   return 0.0;
                if (approximatelyEqual (weight, 1.0))   return 1.0;

                constexpr auto c5 = MathConstants<double>::twoPi / 3.0;

                return 1.0 +
                       (-std::pow (2.0, -10.0 * weight)
                        * std::sin ((weight * 10.0 - 10.75) * c5));
            }

           #if JUCE_CXX14_IS_AVAILABLE
            constexpr double bounce (double weight) noexcept
           #else
            inline double bounce (double weight) noexcept
           #endif
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

        namespace inOut
        {
            constexpr double quad (double weight) noexcept  { return weight < 0.5 ? 2.0  * square (weight)     : 1.0 - square (-2.0 * weight + 2.0) / 2.0; }
            constexpr double cubic (double weight) noexcept { return weight < 0.5 ? 4.0  * cube (weight)       : 1.0 - cube (-2.0 * weight + 2.0) / 2.0; }
            constexpr double quart (double weight) noexcept { return weight < 0.5 ? 8.0  * biquadrate (weight) : 1.0 - biquadrate (-2.0 * weight + 2.0) / 2.0; }
            constexpr double quint (double weight) noexcept { return weight < 0.5 ? 16.0 * sursolid (weight)   : 1.0 - sursolid (-2.0 * weight + 2.0) / 2.0; }

            inline double expo (double weight)
            {
                if (approximatelyEqual (weight, 0.0))   return 0.0;
                if (approximatelyEqual (weight, 1.0))   return 1.0;

                return weight < 0.5
                        ? std::pow (2.0, 20.0 * weight - 10.0) / 2.0
                        : (2.0 - std::pow (2.0, -20.0 * weight + 10.0)) / 2.0;
            }

            inline double circ (double weight)
            {
                return weight < 0.5
                        ? (1.0 - std::sqrt (1.0 - square (2.0 * weight))) / 2.0
                        : (1.0 + std::sqrt (1.0 - square (-2.0 * weight + 2.0))) / 2.0;
            }

            inline double sine (double weight)
            {
                return -(std::cos (MathConstants<double>::pi * weight) - 1.0) / 2.0;
            }

           #if JUCE_CXX14_IS_AVAILABLE
            constexpr double back (double weight) noexcept
           #else
            inline double back (double weight) noexcept
           #endif
            {
                constexpr auto c1 = 1.70158;
                constexpr auto c2 = c1 * 1.525;

                return weight < 0.5
                          ? (square (2.0 * weight)       * ((c2 + 1.0) * 2.0 * weight - c2)) / 2.0
                          : (square (2.0 * weight - 2.0) * ((c2 + 1.0) * (weight * 2.0 - 2.0) + c2) + 2.0) / 2.0;
            }

            /** @returns a value that may be negative! */
            inline double elastic (double weight)
            {
                if (approximatelyEqual (weight, 0.0))   return 0.0;
                if (approximatelyEqual (weight, 1.0))   return 1.0;

                constexpr auto c5 = MathConstants<double>::twoPi / 4.5;

                return weight < 0.5
                        ? -(std::pow (2.0, 20.0 * weight - 10.0) * std::sin ((20.0 * weight - 11.125) * c5)) / 2.0
                        :  (std::pow (2.0, -20.0 * weight + 10.0) * std::sin ((20.0 * weight - 11.125) * c5)) / 2.0 + 1.0;
            }

           #if JUCE_CXX14_IS_AVAILABLE
            constexpr double bounce (double weight) noexcept
           #else
            inline double bounce (double weight) noexcept
           #endif
            {
                 return weight < 0.5
                        ? (1.0 - ease::cubic::out::bounce (1.0 - 2.0 * weight)) / 2.0
                        : (1.0 + ease::cubic::out::bounce (2.0 * weight - 1.0)) / 2.0;
            }
        }

        namespace in
        {
           #if JUCE_CXX14_IS_AVAILABLE
            constexpr double bounce (double weight) noexcept
           #else
            inline double bounce (double weight) noexcept
           #endif
            {
                return 1.0 - ease::cubic::out::bounce (1.0 - std::clamp (weight, 0.0, 1.0));
            }
        }
    }

    namespace audio
    {
        inline double convertWeightToRads (double weight, double frequencyHz = 1.0)
        {
            return weight * frequencyHz * MathConstants<double>::twoPi;
        }

        inline double linear (double weight)            { return weight; }
        inline double smoothstepEase (double weight)    { return smoothstep (0.0, 1.0, weight); }
        inline double smootherstepEase (double weight)  { return smootherstep (0.0, 1.0, weight); }
        inline double sgnEase (double weight)           { return sgn (weight); }
        inline double sinEase (double weight)           { return std::sin (convertWeightToRads (weight)); }
        inline double cosEase (double weight)           { return std::cos (convertWeightToRads (weight)); }
        inline double sincEase (double weight)          { return sinc (convertWeightToRads (weight * 10.0)); }
        inline double squareWave (double weight)        { return weight >= 0.5 ? 1.0 : 0.0; }

        inline double sawtoothWave (double weight)
        {
            return std::acos (std::sin (convertWeightToRads (weight))) / 1.5708;
        }

        inline double triangleWave (double weight)
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
