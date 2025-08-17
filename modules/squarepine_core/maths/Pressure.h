/** A simple class to represent a pressure.

    Internally, this is stored in Pascals, and it can be converted
    to and from a variety of common pressure units.
*/
class Pressure
{
public:
    /** Default constructor, which sets the instance to 101325 Pa. */
    Pressure() noexcept = default;
    /** Copy constructor. */
    Pressure (const Pressure&) noexcept = default;
    /** Move constructor. */
    Pressure (Pressure&&) noexcept = default;
    /** Destructor. */
    ~Pressure() noexcept = default;

    /** Constructor that initialises the pressure with a value in Pascals. */
    explicit constexpr Pressure (double initialPascals) noexcept : pascals (initialPascals) {}

    /** Copy-assignment operator */
    Pressure& operator= (const Pressure&) noexcept = default;
    /** Move-assignment operator */
    Pressure& operator= (Pressure&&) noexcept = default;

    //==============================================================================
    /** @returns the value in Pascals. */
    constexpr double get() const noexcept               { return pascals; }
    /** @returns the value in kPa. */
    constexpr double toKiloPascals() const noexcept     { return pascals / 1000.0; }
    /** @returns the value in mmHg. */
    constexpr double toMillimetersHg() const noexcept   { return pascals * 0.00750062; }
    /** @returns the value in atm. */
    constexpr double toAtmospheres() const noexcept     { return pascals / 101325.0; }
    /** @returns the value in bar. */
    constexpr double toBar() const noexcept             { return pascals / 100000.0; }
    /** @returns the value in PSI. */
    constexpr double toPSI() const noexcept             { return pascals * 0.000145038; }

    //==============================================================================
    /** @returns a pressure value in Pascals from the provided pressure in kPa. */
    static Pressure fromKiloPascals (double value) noexcept     { return Pressure (value * 1000.0); }
    /** @returns a pressure value in Pascals from the provided pressure in mmHg. */
    static Pressure fromMillimetersHg (double value) noexcept   { return Pressure (value * 133.322); }
    /** @returns a pressure value in Pascals from the provided pressure in atm. */
    static Pressure fromAtmospheres (double value) noexcept     { return Pressure (value * 101325.0); }
    /** @returns a pressure value in Pascals from the provided pressure in bar. */
    static Pressure fromBar (double value) noexcept             { return Pressure (value * 100000.0); }
    /** @returns a pressure value in Pascals from the provided pressure in PSI. */
    static Pressure fromPSI (double value) noexcept             { return Pressure (value * 6894.76); }

private:
    //==============================================================================
    double pascals = 101325.0;
};
