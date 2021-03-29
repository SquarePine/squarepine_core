/** Create an instance of this class for using a falsified audio device callback. */
class DummyAudioIODeviceCallback final : public AudioIODeviceCallback
{
public:
    /** Constructor */
    DummyAudioIODeviceCallback();

    //==============================================================================
    /** @internal */
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels, int numSamples) override;
    /** @internal */
    void audioDeviceAboutToStart (AudioIODevice* device) override;
    /** @internal */
    void audioDeviceStopped() override;
    /** @internal */
    void audioDeviceError (const String& errorMessage) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyAudioIODeviceCallback)
};
