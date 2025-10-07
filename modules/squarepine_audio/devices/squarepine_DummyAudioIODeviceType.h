/** Create an instance of this class for using a falsified audio device callback. */
class DummyAudioIODeviceType final : public AudioIODeviceType
{
public:
    /** Constructor. */
    DummyAudioIODeviceType();

    //==============================================================================
    /** @internal */
    void scanForDevices() override;
    /** @internal */
    StringArray getDeviceNames (bool) const override;
    /** @internal */
    int getDefaultDeviceIndex (bool) const override;
    /** @internal */
    int getIndexOfDevice (AudioIODevice*, bool) const override;
    /** @internal */
    bool hasSeparateInputsAndOutputs() const override;
    /** @internal */
    AudioIODevice* createDevice (const String&, const String&) override;

    //==============================================================================
    /** Creates a DummyAudioIODeviceType. */
    static AudioIODeviceType* createAudioIODeviceType_Dummy();

private:
    //==============================================================================
    std::atomic<bool> alreadyScanned { false };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyAudioIODeviceType)
};
