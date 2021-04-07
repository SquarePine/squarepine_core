/** Inherit from this to notify listeners of a deletion happening. */
class DeletionNotifier
{
public:
    /** */
    DeletionNotifier() noexcept = default;

    /** */
    virtual ~DeletionNotifier() noexcept { notifyDeletionListeners(); }

    //==============================================================================
    /** */
    struct DeletionListener
    {
        /** */
        virtual ~DeletionListener() noexcept { }

        /** */
        virtual void handleDeletion (DeletionNotifier* object) = 0;
    };

    /** */
    void addDeletionListener (DeletionListener* listener) { deletionListeners.add (listener); }

    /** */
    void removeDeletionListener (DeletionListener* listener) { deletionListeners.remove (listener); }

private:
    //==============================================================================
    ListenerList<DeletionListener> deletionListeners;

    void notifyDeletionListeners()
    {
        deletionListeners.call ([&] (DeletionListener& l) { l.handleDeletion (this); });
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeletionNotifier)
};
