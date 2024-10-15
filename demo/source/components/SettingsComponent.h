/** */
class SettingsComponent final : public DemoBase
{
public:
    /** */
    SettingsComponent (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Settings"))
    {
        auto* audioDevSel = new AudioDeviceSelectorComponent (sharedObjects.audioDeviceManager,
                                                              0, 2, 0, 2,
                                                              true, true, true, false);
        audioDevSel->setSize (128, 256);
        contents.addAndMakeVisible (comps.add (audioDevSel));

        viewport.setViewedComponent (&contents, false);
        addAndMakeVisible (viewport);
    }

    //==============================================================================
    /** */
    void resized() override
    {
        const auto b = getLocalBounds().reduced (margin);

        const auto compW = b.getWidth() - 1;
        int totalH = 0;
        for (auto& c : comps)
        {
            const auto h = c->getHeight();
            c->setBounds (0, totalH, compW, h);
            totalH += h + margin;
        }

        contents.setSize (compW, totalH);
        viewport.setBounds (b);
    }

private:
    //==============================================================================
    enum { margin = 4 };

    OwnedArray<Component> comps;
    Component contents;
    Viewport viewport;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
};
