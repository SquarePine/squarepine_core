GPSLocation::GPSLocation(float latitude, float longitude, double a) :
    GPSLocation({ latitude, longitude }, a)
{
}

GPSLocation::GPSLocation (Point<float> coords, double a) :
    coordinates (coords),
    altitude (a)
{
}

GPSLocation::GPSLocation (const GPSLocation& other) :
    coordinates (other.coordinates),
    altitude (other.altitude)
{
}

GPSLocation& GPSLocation::operator= (const GPSLocation& other)
{
    coordinates = other.coordinates;
    altitude = other.altitude;
    return *this;
}

GPSLocation& GPSLocation::operator= (GPSLocation&& other)
{
    coordinates = other.coordinates;
    altitude = other.altitude;
    return *this;
}

bool GPSLocation::operator== (const GPSLocation& other) const
{
    return coordinates == other.coordinates
        && altitude == other.altitude;
}

bool GPSLocation::operator!= (const GPSLocation& other) const
{
    return ! operator== (other);
}

bool GPSLocation::isNull() const { return coordinates.isOrigin(); }

#if JUCE_IOS || JUCE_MAC

/**
    Please see GPSLocation.mm for the Apple platform implementation!
*/

#elif JUCE_ANDROID

bool GPSLocation::isLocationAvailable()
{
    if (auto* jniEnv = GetJNIEnv())
    {
        static jmethodID _isLocationAvailable = nullptr;
        if (_isLocationAvailable == nullptr)
            _isLocationAvailable = jniEnv->GetMethodID (GetJNIActivityClass(), "_isLocationAvailable", "()Z");

        return jniEnv->CallBooleanMethod (cachedActivity, _isLocationAvailable);
    }

    return false;
}

std::optional<GPSLocation> GPSLocation::getCurrentLocation()
{
    if (auto* jniEnv = GetJNIEnv())
    {
        static jmethodID _getGPSLocation = nullptr;
        if (_getGPSLocation == nullptr)
            _getGPSLocation = jniEnv->GetMethodID (GetJNIActivityClass(), "_getGPSLocation", "()[D");

        auto javaGPSCurrentLocationResult = (jdoubleArray) jniEnv->CallObjectMethod (cachedActivity, _getGPSLocation);
        if (javaGPSCurrentLocationResult == nullptr)
            return {}; // If this is reached, the GPS permission was probably disabled but it's hard to say exactly...

        auto numItems = jniEnv->GetArrayLength (javaGPSCurrentLocationResult);
        std::vector<double> input (numItems);
        jniEnv->GetDoubleArrayRegion (javaGPSCurrentLocationResult, 0, numItems, &input[0]);

        GPSLocation result;
        decltype (numItems) index = 0;
        if (index < numItems)   result.coordinates.x = (float) input[index++];
        if (index < numItems)   result.coordinates.y = (float) input[index++];
        if (index < numItems)   result.altitude     = (double) input[index++];

        return result;
    }

    return {};
}

#else

bool GPSLocation::isLocationAvailable()                         { return false; }
std::optional<GPSLocation> GPSLocation::getCurrentLocation()    { return {}; }

#endif
