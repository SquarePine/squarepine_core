/**
*/
class TimelineGroup final
{
public:
    /** */
    TimelineGroup() = default;

    //==============================================================================
    void add (WeakReference<Timeline> timeline)
    {
    }

    void remove (WeakReference<Timeline> timeline)
    {
    }

    void remove (int index)
    {
    }

    WeakReference<Timeline> get (int index) const
    {
        return {};
    }

private:
    //==============================================================================

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimelineGroup)
};
