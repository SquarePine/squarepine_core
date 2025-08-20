bool GPSLocation::isLocationAvailable()
{
    return [CLLocationManager locationServicesEnabled]
        && [CLLocationManager authorizationStatus] != kCLAuthorizationStatusDenied
        && [CLLocationManager authorizationStatus] != kCLAuthorizationStatusRestricted;
}

std::optional<GPSLocation> GPSLocation::getCurrentLocation()
{
    if (auto locationManager = [app getLocationManager])
    {
        if ([CLLocationManager authorizationStatus] == kCLAuthorizationStatusNotDetermined)
        {
            [locationManager requestWhenInUseAuthorization];
        }

        if (isLocationAvailable())
        {
            [locationManager requestLocation];

            return
            {
                static_cast<float> (locationManager.location.coordinate.latitude),
                static_cast<float> (locationManager.location.coordinate.longitude),
                static_cast<double> (locationManager.location.altitude)
            };
        }
    }

    return {};
}
