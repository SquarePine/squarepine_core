/** Water phase calculation utilities and thermodynamic property calculations.

    This header provides comprehensive tools for determining water phase states and calculating
    thermodynamic properties of water under various temperature and pressure conditions.

    The implementation includes:
    - Water phase enumeration (solid, liquid, gas, supercritical).
    - Phase determination based on temperature and pressure.
    - Freezing point calculation with pressure dependence.
    - Boiling point calculation using Wagner correlation.
    - High-accuracy saturation pressure calculations.

    All calculations are based on established thermodynamic correlations and empirical
    relationships from water property databases and scientific literature.

    @note Temperature units: Celsius for public API, Kelvin for internal calculations.
    @note Pressure units: Pascals (Pa) throughout.
    @note Valid operating ranges are defined by water's triple and critical points.

    @see https://en.wikipedia.org/wiki/Properties_of_water
    @see Wagner, W., & Pruß, A. (2002). The IAPWS formulation 1995 for the thermodynamic properties of ordinary water substance for general and scientific use.
 */

/** Represents the four primary states of of water based on
    temperature and pressure conditions according to the water phase diagram.
 */
enum class WaterPhase
{
    solid,          // Ice phase (below freezing point).
    liquid,         // Liquid water phase (between freezing and boiling points).
    gas,            // Water vapour/steam phase (above boiling point).
    supercritical   // Supercritical fluid phase (above critical temperature and pressure).
};

/** @returns a localised string representation of the water phase. */
inline String toString (WaterPhase mp) noexcept
{
    switch (mp)
    {
        case WaterPhase::solid:          return TRANS ("Solid");
        case WaterPhase::liquid:         return TRANS ("Liquid");
        case WaterPhase::gas:            return TRANS ("Gas");
        case WaterPhase::supercritical:  return TRANS ("Supercritical Fluid");
        default:                         return TRANS ("Unknown");
    }
}

//==============================================================================
/** A utility class for calculating water phase transitions and properties.

    This class provides static methods for determining water phase states based on
    temperature and pressure, calculating freezing and boiling points under various
    pressure conditions, and implementing thermodynamic correlations for water.

    The calculations are based on established thermodynamic relationships including:
    - Linear approximation for freezing point depression.
    - Wagner correlation for saturation pressure.
    - Critical point properties for supercritical phase determination.

    @note All temperature inputs and outputs are in Celsius unless otherwise specified.
    @note All pressure inputs are in Pascals (Pa).

    @see WaterPhase
 */
class WaterPhaseCalculator final
{
public:
    /** Calculates the freezing point of water at a given pressure.

        Uses a linear approximation for freezing point depression based on pressure.
        The relationship assumes a slope of -7.4×10⁻⁸ K/Pa near 1 atmosphere.

        @param pressurePa The pressure in Pascals.

        @returns the freezing point temperature in degrees Celsius.

        @note This is a linear approximation valid for moderate pressure ranges.
        @note At standard atmospheric pressure (101325 Pa), returns 0°C.
     */
    static constexpr double getFreezingPointCelsius (Pressure pressure) noexcept
    {
        constexpr auto slope = -7.4e-8;     // K/Pa near 1 atm
        constexpr auto pref = kPaPerAtm;    // Left here in case configuration is necessary.
        constexpr auto TrefC = 0.0;         // Left here in case configuration is necessary.

        return TrefC + slope * (pressure - pref);
    }

    /** Calculates the boiling point of water at a given pressure using Wagner correlation.

        This method uses the Wagner equation for saturation pressure and employs a bisection
        algorithm to find the temperature at which the saturation pressure equals the given pressure.

        @param pressurePa   The pressure in Pascals.
        @param tolK         Temperature tolerance for convergence in Kelvin (default: 1e-5).
        @param maxIters     Maximum number of iterations for the bisection algorithm (default: 100).

        @return The boiling point temperature in degrees Celsius, or std::nullopt if:
                - Pressure is below triple point pressure.
                - Pressure is above critical pressure.
                - Convergence fails.

        @note Valid pressure range: 611.657 Pa (triple point) to 22.064×10⁶ Pa (critical point).
        @note Uses bisection method for numerical solution.
     */
    static std::optional<double> getBoilingPointCelsius (Pressure pressure,
                                                         double tolK = 1e-5,
                                                         int maxIters = 100)
    {
        if (pressure < Ptriple || pressure >= Pc)
            return std::nullopt;

        auto lo = Ttriple;
        auto hi = Tc - 1e-6;

        const auto PloOpt = saturationPressurePa_Wagner (lo);
        const auto PhiOpt = saturationPressurePa_Wagner (hi);
        if (! PloOpt || ! PhiOpt)
            return std::nullopt;

        const auto Plo = *PloOpt;
        const auto Phi = *PhiOpt;

        if (! (Plo <= pressure && pressure <= Phi))
            return std::nullopt;

        for (int i = 0; i < maxIters && (hi - lo) > tolK; ++i)
        {
            const auto mid = 0.5 * (lo + hi);
            const auto PmidOpt = saturationPressurePa_Wagner(mid);

            if (! PmidOpt)
                return std::nullopt;

            const auto Pmid = *PmidOpt;
            if (Pmid < pressure)    lo = mid;
            else                    hi = mid;
        }

        const auto T_K = 0.5 * (lo + hi);
        return T_K - 273.15;
    }

    /** Determines the phase of water given temperature and pressure conditions.

        This method analyses the thermodynamic state of water based on input conditions
        and returns the corresponding phase according to the water phase diagram.

        The determination logic:
        - Below triple point pressure: solid or gas (sublimation).
        - Above critical point: supercritical fluid.
        - Between triple and critical points: solid, liquid, or gas based on phase boundaries.

        @param tempC        Temperature in degrees Celsius.
        @param pressurePa   Pressure in Pascals.

        @returns the corresponding WaterPhase enumeration value.

        @note Uses calculated freezing and boiling points for phase boundary determination.
        @note Handles edge cases like triple point and critical point conditions.

        @see WaterPhase
        @see getFreezingPointCelsius()
        @see getBoilingPointCelsius()
     */
    static WaterPhase getWaterPhase (double tempC, Pressure pressure)
    {
        if (pressure < Ptriple)
            return (tempC < (Ttriple - 273.15))
                    ? WaterPhase::solid
                    : WaterPhase::gas;

        if (pressure >= Pc && (tempC + 273.15) >= Tc)
            return WaterPhase::supercritical;

        const auto TfC = getFreezingPointCelsius (pressure);
        if (tempC < TfC)
            return WaterPhase::solid;

        const auto TbOpt = getBoilingPointCelsius (pressure);
        if (! TbOpt)
            return (tempC >= (Tc - 273.15))
                    ? WaterPhase::supercritical
                    : WaterPhase::liquid;

        if (tempC < *TbOpt)
            return WaterPhase::liquid;

        return WaterPhase::gas;
    }

private:
    //==============================================================================
    // Physical constants for water.
    static inline constexpr double kPaPerAtm = 101325.0;    // Conversion factor: kPa per atmosphere.
    static inline constexpr double Tc = 647.096;            // Critical temperature in Kelvin (374.0°C).
    static inline constexpr double Pc = 22.064e6;           // Critical pressure in Pascals (220.64 bar).
    static inline constexpr double Ttriple = 273.16;        // Triple point temperature in Kelvin (0.01°C).
    static inline constexpr double Ptriple = 611.657;       // Triple point pressure in Pascals.

    /** Calculates saturation pressure using the Wagner correlation.

        The Wagner equation is a highly accurate correlation for calculating the saturation
        pressure of water as a function of temperature. It uses a polynomial expression
        involving the reduced temperature (τ = 1 - T/Tc).

        The equation form is:
        ln(P/Pc) = (Tc/T) × [a×τ + b×τ^1.5 + c×τ^3 + d×τ^3.5 + e×τ^4 + f×τ^7.5]

        @param T_K Temperature in Kelvin
        @returns the saturation pressure in Pascals, or std::nullopt
         if temperature is outside the valid range (triple point to critical point).

        @note Valid temperature range: 273.16 K to 647.096 K
        @note Wagner coefficients are empirically determined constants
        @note This correlation is considered one of the most accurate for water
     */
    static std::optional<double> saturationPressurePa_Wagner (double T_K)
    {
        if (T_K < Ttriple || T_K > Tc)
            return std::nullopt;

        // Wagner correlation coefficients (empirically determined)
        constexpr auto a = -7.85951783;
        constexpr auto b = 1.84408259; 
        constexpr auto c = -11.7866497;
        constexpr auto d = 22.6807411; 
        constexpr auto e = -15.9618719;
        constexpr auto f = 1.80122502; 

        const auto tau = 1.0 - T_K / Tc;
        const auto poly = a * tau
                        + b * std::pow (tau, 1.5)
                        + c * std::pow (tau, 3.0)
                        + d * std::pow (tau, 3.5)
                        + e * std::pow (tau, 4.0)
                        + f * std::pow (tau, 7.5);

        const auto lnPratio = (Tc / T_K) * poly;
        return Pc * std::exp (lnPratio);
    }
};
