/** */
class AnimationDemo final : public DemoBase
{
public:
    /** */
    AnimationDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Animations"))
    {
    }

    //==============================================================================
    void resized() override
    {
        auto b = getLocalBounds().reduced (dims::marginPx);

        ignoreUnused (b);
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimationDemo)
};
