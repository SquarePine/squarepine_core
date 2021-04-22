DummyAudioIODeviceCallback::DummyAudioIODeviceCallback()
{
}

//==============================================================================
void DummyAudioIODeviceCallback::audioDeviceIOCallback (const float** const, const int, float** const outputChannelData,
                                                        const int numOutputChannels, const int numSamples)
{
    juce::AudioBuffer<float> buffer;
    buffer.setDataToReferTo (outputChannelData, numOutputChannels, numSamples);
    buffer.clear();
}

void DummyAudioIODeviceCallback::audioDeviceAboutToStart (AudioIODevice*)
{
}

void DummyAudioIODeviceCallback::audioDeviceStopped()
{
}

void DummyAudioIODeviceCallback::audioDeviceError (const String&)
{
}
