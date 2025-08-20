/** Represents a GPS location that provides the coordinates and altitude.

    Call isLocationAvailable to find out if the system can provide you a location,
    and then call getCurrentLocation to get the system's location.
*/
class GPSLocation final
{
public:
    /** Represents a null GPS location.

        @see isNull
    */
    GPSLocation() = default;

    /** Creates a GPS location based on the provided coordinates and altitude. */
    GPSLocation (float latitude, float longitude, double altitude = 0.0);

    /** Creates a GPS location based on the provided coordinates and altitude. */
    GPSLocation (Point<float> coordinates, double altitude = 0.0);

    /** Copy constructor.  */
    GPSLocation (const GPSLocation&) = default;

    /** Move constructor.  */
    GPSLocation (GPSLocation&&) = default;

    /** Destructor. */
    ~GPSLocation() = default;

    /** @returns true if the location can be retrieved.

        This can return false if the platform doesn't have location support
        or has location explicitly disabled.
    */
    static bool isLocationAvailable();

    /** @returns the current location of the platform's GPS.
        The instance returned might be null if this feature is not available
        due to platform limitations or lacking platform permissions.
    */
    static std::optional<GPSLocation> getCurrentLocation();

    /** @returns true if the location is null (latitude: 0.0, longitude: 0.0).

        The vertical axis is irrespective of the location and so the
        altitude is not considered part of the null-ness.

        Technically this location is valid, but may indicate an issue outside of the
        underlying platform APIs, such as the GPS being unable to get a fix on the location:
        @see https://en.wikipedia.org/wiki/Null_Island
    */
    bool isNull() const;

    GPSLocation& operator= (const GPSLocation&);
    GPSLocation& operator= (GPSLocation&&);
    bool operator== (const GPSLocation&) const;
    bool operator!= (const GPSLocation&) const;

    Point<float> coordinates;   //< The geographic location, where x is the latitude and y is the longitude.
    double altitude = 0.0;      //< The altitude. Though typically this is in meters, it may be platform dependant.
};