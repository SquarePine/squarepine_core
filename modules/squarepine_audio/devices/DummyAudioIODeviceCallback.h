/** Create an instance of this class for using a falsified audio device callback. */
class DummyAudioIODeviceCallback final : public AudioIODeviceCallback
{
public:
    /** Constructor */
    DummyAudioIODeviceCallback();

    //==============================================================================
    /** @internal */
    void audioDeviceIOCallback (const float**, int, float**, int, int) override;
    /** @internal */
    void audioDeviceAboutToStart (AudioIODevice*) override;
    /** @internal */
    void audioDeviceStopped() override;
    /** @internal */
    void audioDeviceError (const String&) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyAudioIODeviceCallback)
};
