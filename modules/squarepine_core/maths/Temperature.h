/** A simple class to represent a temperature.

    Internally, this is stored in Celsius, and it can be converted
    to and from a variety of common temperature units.
*/
class Temperature
{
public:
    /** Default constructor, which sets the instance to 0 degrees Celsius. */
    Temperature() noexcept = default;
    /** Copy constructor. */
    Temperature (const Temperature&) noexcept = default;
    /** Move constructor. */
    Temperature (Temperature&&) noexcept = default;
    /** Destructor. */
    ~Temperature() noexcept = default;

    /** Constructor that initialises the temperature with a value in Celsius. */
    explicit Temperature (double initialCelsius) noexcept : celsius (initialCelsius) {}

    /** Copy-assignment operator */
    Temperature& operator= (const Temperature&) noexcept = default;
    /** Move-assignment operator */
    Temperature& operator= (Temperature&&) noexcept = default;

    //==============================================================================
    /** @returns the value in Celsius. */
    constexpr double get() const noexcept           { return celsius; }
    /** @returns the value in Fahrenheit. */
    constexpr double toFahrenheit() const noexcept  { return (celsius * 9.0 / 5.0) + 32.0; }
    /** @returns the value in Kelvin. */
    constexpr double toKelvin() const noexcept      { return celsius + 273.15; }

    /** @returns a temperature value in Celsius from the provided Fahrenheit temperature. */
    static Temperature fromFahrenheit (double value) noexcept   { return Temperature ((value - 32.0) * 5.0 / 9.0); }
    /** @returns a temperature value in Celsius from the provided Kelvin temperature. */
    static Temperature fromKelvin (double value) noexcept       { return Temperature (value - 273.15); }

private:
    //==============================================================================
    double celsius = 0.0;
};
