/** An element comparator used for sorting files in an ascending or descending manner. */
class FileSorter final
{
public:
    FileSorter (bool ascending = true, bool shouldBeCaseSensitive = false) noexcept :
        mult (ascending ? 1 : -1),
        caseSensitive (shouldBeCaseSensitive)
    {
    }

    int compareElements (const String& firstPath, const String& secondPath) const noexcept
    {
        return firstPath.compareNatural (secondPath, caseSensitive) * mult;
    }

    int compareElements (const File& first, const File& second) const noexcept
    {
        return compareElements (first.getFullPathName(), second.getFullPathName());
    }

private:
    const int mult;
    const bool caseSensitive;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileSorter)
};

//==============================================================================
/** This is a basic Timer wrapper where the provided callback function will be called.

    Simply put, this prevents having to constantly derive a Timer to use one!
*/
class OffloadedTimer final : public Timer
{
public:
    OffloadedTimer() = default;

    void timerCallback() override
    {
        if (callback != nullptr)
            callback();
    }

    std::function<void()> callback;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OffloadedTimer)
};

/** This is basic HighResolutionTimer wrapper where the provided callback function will be called.

    Simply put, this prevents having to constantly derive a HighResolutionTimer to use one!
*/
class OffloadedTimerHighRes final : public HighResolutionTimer
{
public:
    OffloadedTimerHighRes() = default;

    void hiResTimerCallback() override
    {
        if (callback != nullptr)
            callback();
    }

    std::function<void()> callback;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OffloadedTimerHighRes)
};

//==============================================================================
class AccurateTimer final : public Thread,
                            public Lockable
{
public:
    AccurateTimer() :
        Thread ("AccurateTimerTM")
    {
    }

    ~AccurateTimer() override
    {
        shutdownThreadSafely (*this);
    }

    //==============================================================================
    void startTimer (double newIntervalSeconds) 
    {
        intervalSeconds.store (jmax (newIntervalSeconds, 0.0));

        if (! isThreadRunning())
            startThread();
    }

    void startTimer (int intervalInMilliseconds) 
    {
        startTimer (RelativeTime::milliseconds (intervalInMilliseconds).inSeconds());
    }

    void startTimerHz (int timerFrequencyHz) 
    {
        startTimer (1000.0 / static_cast<double> (timerFrequencyHz) / 1000.0);
    }

    void stopTimer()
    {
        shutdownThreadSafely (*this);
    }

    double getIntervalSeconds() const noexcept { return intervalSeconds.load(); }

    //==============================================================================
    std::function<void()> callback;

private:
    //==============================================================================
    std::atomic<double> intervalSeconds { 0.0 };

    void run() override
    {
        auto lastTimeTicks = Time::getHighResolutionTicks();

        while (! threadShouldExit() && intervalSeconds.load() > 0.0)
        {
            {
                const ScopedLock sl (lock);
                if (callback != nullptr)
                    callback();
            }

            const auto sleepTime = Time::secondsToHighResolutionTicks (intervalSeconds.load());
            lastTimeTicks += sleepTime;
            waitUntilTime (lastTimeTicks, 1);
        }
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccurateTimer)
};

//==============================================================================
JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wall", "-Wconversion", "-Wimplicit-const-int-float-conversion", "-Wsign-conversion")

/** */
template<template<typename, typename> class Container, 
         typename Value,
         typename Allocator = std::allocator<Value>,
         typename IndexType>
inline bool moveItem (Container<Value, Allocator>& v, IndexType oldIndex, IndexType newIndex)
{
    const auto oldI = static_cast<size_t> (oldIndex);
    const auto newI = static_cast<size_t> (newIndex);

    if (oldI > v.size() || newI > v.size())
    {
        jassertfalse;
        return false;
    }

    if (oldI > newI)
        std::rotate (v.rend() - oldI - 1, v.rend() - oldI, v.rend() - newI);
    else        
        std::rotate (v.begin() + oldI, v.begin() + oldI + 1, v.begin() + newI + 1);

    return true;
}

/** */
template<template<typename, typename> class Container, 
         typename Value,
         typename Allocator = std::allocator<Value>,
         typename IndexType>
inline bool moveItemToBack (Container<Value, Allocator>& v, IndexType itemIndex)
{
    const auto i = static_cast<size_t> (itemIndex);
    if (i > v.size())
    {
        jassertfalse;
        return false;
    }

    auto it = v.begin() + i;
    std::rotate (it, it + 1, v.end());
    return true;
}

/** */
template<template<typename, typename> class Container, 
         typename Value,
         typename Allocator = std::allocator<Value>,
         typename IndexType>
inline bool moveItemToFront (Container<Value, Allocator>& v, IndexType itemIndex)
{
    const auto i = static_cast<size_t> (itemIndex);
    if (i > v.size())
    {
        jassertfalse;
        return false;
    }

    auto b = v.begin();
    auto it = b + i;
    std::rotate (b, it, it + 1);
    return true;
}

/** */
template<template<typename, typename> class Container, 
         typename Value,
         typename Allocator = std::allocator<Value>,
         typename IndexType>
inline bool removeItem (Container<Value, Allocator>& v, IndexType itemIndex)
{
    const auto index = static_cast<size_t> (itemIndex);
    if (index > v.size())
    {
        jassertfalse;
        return false;
    }

    v.erase (v.begin() + index);
    return true;
}

/** */
template<template<typename, typename> class Container, 
         typename Value,
         typename Allocator = std::allocator<Value>,
         typename IndexType,
         typename ItemType>
inline bool replaceItem (Container<Value, Allocator>& v, IndexType itemIndex, ItemType& newItem)
{
    const auto i = static_cast<size_t> (itemIndex);
    if (i > v.size())
    {
        jassertfalse;
        return false;
    }

    v[i] = newItem;
    return true;
}

/** */
template<template<typename, typename> class Container, 
         typename Value,
         typename Allocator = std::allocator<Value>>
inline void append (Container<Value, Allocator>& destination, const Container<Value, Allocator>& source)
{
    destination.insert (std::end (destination), std::begin (source), std::end (source));
}

/** */
template<template<typename, typename> class Container, 
         typename Value,
         typename Allocator = std::allocator<Value>,
         typename Predicate>
inline Value removeAndReturn (Container<Value, Allocator>& container, Predicate predicate)
{
    auto result = Value();

    auto iter = std::remove_if (container.begin(), container.end(),
    [&] (Value& value)
    {
        return predicate (result, value);
    });

    container.erase (iter, container.cend());

    return result;
}

/** */
template<template<typename, typename> class Container, 
         typename Value,
         typename Allocator = std::allocator<Value>>
inline void removeAllInstancesOf (Container<Value, Allocator>& container, const Value& value)
{
    container.erase (std::remove (container.begin(), container.end(), value), container.cend());
}

/** */
template<template<typename, typename> class Container, 
         typename Value,
         typename Allocator = std::allocator<Value>>
inline bool contains (const Container<Value, Allocator>& container, const Value& value)
{
    return std::find (container.cbegin(), container.cend(), value) != container.cend();
}

JUCE_END_IGNORE_WARNINGS_GCC_LIKE

//==============================================================================
/** */
template<typename ObjectType>
inline String getDemangledName (ObjectType& c)
{
   #if ! JUCE_DEBUG || JUCE_MINGW
    ignoreUnused (c);
    return {};
   #elif JUCE_WINDOWS && ! JUCE_MINGW
    auto res = String (typeid (c).name());

    if (res.startsWith ("class "))
        res = res.substring (6);
    if (res.startsWith ("struct "))
        res = res.substring (7);

    return res;
   #else
    using namespace abi;
    int status = 0;
    if (auto* demangled = __cxa_demangle (typeid (c).name(), nullptr, nullptr, &status))
    {
        auto res = String (demangled);
        free (demangled);
        return res;
    }
    return {};
   #endif
}

/** */
template<typename ObjectType>
inline String getDemangledName (ObjectType* c)
{
    if (c != nullptr)
        return getDemangledName (*c);

    return "nullptr";
}

//==============================================================================
/** */
inline int getMaxPathLength()
{
    int maxLength = 128;

   #if JUCE_WINDOWS
    const auto v = WindowsRegistry::getValue ("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\FileSystem\\LongPathsEnabled").trim();

    if (v == "1")
        maxLength = 256;
   #elif defined (NAME_MAX)
    maxLength = roundToInt ((double) NAME_MAX / 3.0) - 1;
   #elif defined (PATH_MAX)
    maxLength = roundToInt ((double) PATH_MAX / 3.0) - 1;
   #endif

    return jmax (128, maxLength);
}
