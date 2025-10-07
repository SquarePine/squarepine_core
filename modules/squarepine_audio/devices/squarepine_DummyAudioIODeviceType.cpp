DummyAudioIODeviceType::DummyAudioIODeviceType() :
    AudioIODeviceType ("Dummy")
{
}

//==============================================================================
void DummyAudioIODeviceType::scanForDevices()
{
    if (! alreadyScanned)
    {
        callDeviceChangeListeners();
        alreadyScanned = true;
    }
}

StringArray DummyAudioIODeviceType::getDeviceNames (const bool wantInputNames) const
{
    StringArray devices;

    if (wantInputNames)
        devices.add ("Dummy Input");
    else
        devices.add ("Dummy Output");

    devices.minimiseStorageOverheads();
    return devices;
}

int DummyAudioIODeviceType::getDefaultDeviceIndex (const bool /*forInput*/) const
{
    return 0;
}

int DummyAudioIODeviceType::getIndexOfDevice (AudioIODevice* device, const bool /*asInput*/) const
{
    return device != nullptr ? 0 : -1;
}

bool DummyAudioIODeviceType::hasSeparateInputsAndOutputs() const
{
    return true;
}

AudioIODevice* DummyAudioIODeviceType::createDevice (const String& outputDeviceName,
                                                     const String& inputDeviceName)
{
    callDeviceChangeListeners();

    if (outputDeviceName.isNotEmpty() || inputDeviceName.isNotEmpty())
        return new DummyAudioIODevice (outputDeviceName.isEmpty());

    return nullptr;
}

//==============================================================================
AudioIODeviceType* DummyAudioIODeviceType::createAudioIODeviceType_Dummy()
{
    return new DummyAudioIODeviceType();
}
