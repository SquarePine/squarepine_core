/** Derive from this to create a lockable class that uses a form of JUCE lock. */
template<typename TypeOfCriticalSectionToUse>
class LockableBase
{
public:
    /** */
    LockableBase() noexcept = default;

    /** */
    virtual ~LockableBase() noexcept = default;

    //==============================================================================
    /** The type of lock to use for locking. */
    using Lock = TypeOfCriticalSectionToUse;
    /** The type of lock to use for automatically locking and unlocking using RAII. */
    using ScopedLock = GenericScopedLock<Lock>;
    /** The type of lock to use for automatically unlocking and relocking using RAII. */
    using ScopedUnlockType = GenericScopedUnlock<Lock>;
    /** The type of lock to use for automatically try locking (and unlocking) using RAII. */
    using ScopedTryLockType = GenericScopedTryLock<Lock>;

    /** @returns the Lock that locks this lockable.

        To lock, you can call getLock().enter() and getLock().exit(),
        or preferably use an RAII alternative to manage lock and unlocking.
    */
    [[nodiscard]] Lock& getLock() noexcept { return lock; }

protected:
    //==============================================================================
    Lock lock;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LockableBase)
};

//==============================================================================
/** Derive from this to create a lockable class that uses an std::recursive_mutex. */
template<>
class LockableBase<std::recursive_mutex>
{
public:
    /** */
    LockableBase() noexcept = default;

    /** */
    virtual ~LockableBase() noexcept = default;

    //==============================================================================
    /** The type of lock to use for locking. */
    using Lock = std::recursive_mutex;

    /** The type of lock to use for automatically locking and unlocking using RAII. */
    using ScopedLock = std::scoped_lock<Lock>;

    /** @returns the Lock that locks this lockable.

        To lock, you can call getLock().enter() and getLock().exit(),
        or preferably use an RAII alternative to manage lock and unlocking.
    */
    [[nodiscard]] Lock& getLock() noexcept { return lock; }

protected:
    //==============================================================================
    Lock lock;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LockableBase)
};

//==============================================================================
/** Derive from this to create a lockable class that uses a CriticalSection. */
using Lockable = LockableBase<CriticalSection>;
