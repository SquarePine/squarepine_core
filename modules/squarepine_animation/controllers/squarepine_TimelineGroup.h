/**
*/
class TimelineGroup final
{
public:
    /** */
    TimelineGroup() = default;

    //==============================================================================
    void add (WeakReference<Timeline>)
    {
    }

    void remove (WeakReference<Timeline>)
    {
    }

    void remove (int)
    {
    }

    WeakReference<Timeline> get (int) const
    {
        return {};
    }

private:
    //==============================================================================

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimelineGroup)
};
