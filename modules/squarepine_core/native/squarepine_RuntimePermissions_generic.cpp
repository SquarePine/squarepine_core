#if ! JUCE_ANDROID

void RuntimePermissions::request (const String&, Callback callback)         { callback (true); }
void RuntimePermissions::request (const StringArray&, Callback callback)    { callback (true); }
bool RuntimePermissions::isRequired (const String&)                         { return false; }
bool RuntimePermissions::isRequired (const StringArray&)                    { return false; }
bool RuntimePermissions::isGranted (const String&)                          { return true; }
bool RuntimePermissions::isGranted (const StringArray&)                     { return true; }

#endif // JUCE_ANDROID
