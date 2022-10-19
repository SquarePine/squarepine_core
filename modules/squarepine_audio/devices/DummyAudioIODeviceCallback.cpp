DummyAudioIODeviceCallback::DummyAudioIODeviceCallback()
{
}

//==============================================================================
void DummyAudioIODeviceCallback::audioDeviceIOCallbackWithContext (const float**,
                                                                   int,
                                                                   float** outputChannelData,
                                                                   int numOutputChannels,
                                                                   int numSamples,
                                                                   const AudioIODeviceCallbackContext&)
{
    juce::AudioBuffer<float> buffer;
    buffer.setDataToReferTo (outputChannelData, numOutputChannels, numSamples);
    buffer.clear();
}
