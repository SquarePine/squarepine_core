/** */
class MediaDeviceListerDemo final : public DemoBase,
                                    public MediaDevicePoller::Listener
{
public:
    /** */
    MediaDeviceListerDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Media Device Lister")),
        devicePoller (sharedObjs.audioDeviceManager)
    {
        devicePoller.addListener (this);

        audioDeviceInfo.setMultiLine (true, false);
        midiDeviceInfo.setMultiLine (true, false);

        addAndMakeVisible (audioDeviceInfo);
        addAndMakeVisible (midiDeviceInfo);

        update();
    }

    /** */
    ~MediaDeviceListerDemo() override
    {
        devicePoller.removeListener (this);
    }

    //==============================================================================
    /** @internal */
    void resized() override
    {
        constexpr auto margin = 4;

        auto b = getLocalBounds().reduced (margin);

        auto tb = b.removeFromTop ((b.getHeight() / 2) - (margin / 2));
        audioDeviceInfo.setBounds (tb);

        b.removeFromTop (margin);
        midiDeviceInfo.setBounds (b);
    }

    /** @internal */
    void driverChanged (const String&) override { update(); }
    /** @internal */
    void numInputsChanged (int, bool) override  { update(); }
    /** @internal */
    void numOutputsChanged (int, bool) override { update(); }
    /** @internal */
    void deviceAdded (const String&) override   { update(); }
    /** @internal */
    void deviceRemoved (const String&) override { update(); }
    /** @internal */
    void updateWithNewTranslations() override   { update(); }

private:
    //==============================================================================
    TextEditor audioDeviceInfo, midiDeviceInfo;
    MediaDevicePoller devicePoller;
    const String separator { String::repeatedString ("-", 30) };

    //==============================================================================
    void update()
    {
        {
            StringArray details;
            details.add (toUpperCase (TRANS ("Input Devices")) + newLine);
            details.addArray (devicePoller.getAudioInputDevices());
            details.add (separator);
            details.add (toUpperCase (TRANS ("Output Devices")) + newLine);
            details.addArray (devicePoller.getAudioOutputDevices());
            details.add (separator);
            details.add (toUpperCase (TRANS ("Input Channel Names")) + newLine);
            details.addArray (devicePoller.getInputChannelNames());
            details.add (separator);
            details.add (toUpperCase (TRANS ("Output Channel Names")) + newLine);
            details.addArray (devicePoller.getOutputChannelNames());

            audioDeviceInfo.setText (details.joinIntoString (newLine));
        }

        {
            Array<MidiDeviceInfo> combinedInfo;
            combinedInfo.addArray (devicePoller.getMIDIInputDevices());
            combinedInfo.addArray (devicePoller.getMIDIOutputDevices());

            StringArray details;
            for (const auto& info : combinedInfo)
            {
                details.add (info.name);
                details.add (info.identifier);
                details.add (separator);
            }

            midiDeviceInfo.setText (details.joinIntoString (newLine));
        }
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MediaDeviceListerDemo)
};
