/** */
class ProgramAudioProcessorEditor final : public AudioProcessorEditor
{
public:
    /** */
    ProgramAudioProcessorEditor (AudioProcessor& p);

    //==============================================================================
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void resized() override;

private:
    //==============================================================================
    struct PropertyComp final : public PropertyComponent,
                                private AudioProcessorListener
    {
        PropertyComp (const String& name, AudioProcessor& p) :
            PropertyComponent (name),
            owner (p)
        {
            owner.addListener (this);
        }

        ~PropertyComp() override
        {
            owner.removeListener (this);
        }

        /** @internal */
        void refresh() override {}
        /** @internal */
        void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override {}
        /** @internal */
        void audioProcessorParameterChanged (AudioProcessor*, int, float) override {}

        AudioProcessor& owner;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyComp)
    };

    //==============================================================================
    PropertyPanel panel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgramAudioProcessorEditor)
};
