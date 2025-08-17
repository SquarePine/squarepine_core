
/** The phase of water. */
enum class WaterPhase
{
    solid,
    liquid,
    gas,
    supercritical
};

/** @returns the String representation of the given water's phase. */
inline String toString (WaterPhase mp) noexcept
{
    switch (mp)
    {
        case WaterPhase::solid:          return TRANS ("Solid");
        case WaterPhase::liquid:         return TRANS ("Liquid");
        case WaterPhase::gas:            return TRANS ("Gas");
        case WaterPhase::supercritical:  return TRANS ("Supercritical Fluid");
        default:                            return TRANS ("Unknown");
    }
}

class WaterPhaseCalculator final
{
public:
    /** Freezing point (°C) correlated with pressure (Pa). */
    static double getFreezingPointCelsius (double pressurePa)
    {
        constexpr auto slope = -7.4e-8;       // K/Pa near 1 atm
        constexpr auto Pref = kPaPerAtm;
        constexpr auto TrefC = 0.0;
        return TrefC + slope * (pressurePa - Pref);
    }

    /** Boiling point (°C) vs pressure (Pa) using Wagner correlation. */
    static std::optional<double> getBoilingPointCelsius (double pressurePa,
                                                         double tolK = 1e-5,
                                                         int maxIters = 100)
    {
        if (pressurePa < Ptriple || pressurePa >= Pc)
            return std::nullopt;

        auto lo = Ttriple;
        auto hi = Tc - 1e-6;
        auto PloOpt = saturationPressurePa_Wagner (lo);
        auto PhiOpt = saturationPressurePa_Wagner (hi);
        if (!PloOpt || !PhiOpt)
            return std::nullopt;

        auto Plo = *PloOpt;
        auto Phi = *PhiOpt;

        if (!(Plo <= pressurePa && pressurePa <= Phi))
            return std::nullopt;

        for (int i = 0; i < maxIters && (hi - lo) > tolK; ++i)
        {
            auto mid = 0.5 * (lo + hi);
            auto PmidOpt = saturationPressurePa_Wagner(mid);

            if (!PmidOpt)
                return std::nullopt;

            auto Pmid = *PmidOpt;
            if (Pmid < pressurePa) lo = mid;
            else                   hi = mid;
        }

        const auto T_K = 0.5 * (lo + hi);
        return T_K - 273.15;
    }

    /** Phase determination. */
    static WaterPhase getWaterPhase (double tempC, double pressurePa)
    {
        if (pressurePa < Ptriple)
            return (tempC < (Ttriple - 273.15))
                    ? WaterPhase::solid
                    : WaterPhase::gas;

        if (pressurePa >= Pc && (tempC + 273.15) >= Tc)
            return WaterPhase::supercritical;

        const auto TfC = getFreezingPointCelsius (pressurePa);
        if (tempC < TfC)
            return WaterPhase::solid;

        const auto TbOpt = getBoilingPointCelsius (pressurePa);
        if (!TbOpt)
            return (tempC >= (Tc - 273.15))
                    ? WaterPhase::supercritical
                    : WaterPhase::liquid;

        if (tempC < *TbOpt)
            return WaterPhase::liquid;

        return WaterPhase::gas;
    }

private:
    static inline constexpr double kPaPerAtm = 101325.0;
    static inline constexpr double Tc = 647.096;      // K
    static inline constexpr double Pc = 22.064e6;     // Pa
    static inline constexpr double Ttriple = 273.16;  // K
    static inline constexpr double Ptriple = 611.657; // Pa

    /** Wagner saturation pressure. */
    static std::optional<double> saturationPressurePa_Wagner (double T_K)
    {
        if (T_K < Ttriple || T_K > Tc)
            return std::nullopt;

        constexpr auto a = -7.85951783;
        constexpr auto b =  1.84408259;
        constexpr auto c = -11.7866497;
        constexpr auto d =  22.6807411;
        constexpr auto e = -15.9618719;
        constexpr auto f =   1.80122502;

        auto tau = 1.0 - T_K / Tc;
        auto poly = a * tau
                    + b * std::pow (tau, 1.5)
                    + c * std::pow (tau, 3.0)
                    + d * std::pow (tau, 3.5)
                    + e * std::pow (tau, 4.0)
                    + f * std::pow (tau, 7.5);

        const auto lnPratio = (Tc / T_K) * poly;
        return Pc * std::exp (lnPratio);
    }
};
