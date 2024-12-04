/** */
class AndroidPermissionDemo final : public DemoBase
{
public:
    /** */
    AndroidPermissionDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Android Permissions"))
    {
    }

    //==============================================================================
    /** @internal */
    void resized() override
    {
    }

    /** @internal */
    void updateWithNewTranslations() override
    {
        repaint();
    }

   #if JUCE_ANDROID
   #endif // JUCE_ANDROID

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AndroidPermissionDemo)
};
