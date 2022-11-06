/** An element comparator used for sorting files in an ascending or descending manner. */
class FileSorter final
{
public:
    /** */
    FileSorter (bool ascending = true, bool shouldBeCaseSensitive = false) noexcept :
        mult (ascending ? 1 : -1),
        caseSensitive (shouldBeCaseSensitive)
    {
    }

    /** */
    int compareElements (const String& firstPath, const String& secondPath) const noexcept
    {
        return firstPath.compareNatural (secondPath, caseSensitive) * mult;
    }

    /** */
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
    /** */
    OffloadedTimer() = default;

    /** */
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
    /** */
    OffloadedTimerHighRes() = default;

    /** */
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
/** */
class AccurateTimer final : public Thread,
                            public Lockable
{
public:
    /** */
    AccurateTimer() :
        Thread ("AccurateTimerTM")
    {
    }

    /** */
    ~AccurateTimer() override
    {
        shutdownThreadSafely (*this);
    }

    //==============================================================================
    /** */
    void startTimer (double newIntervalSeconds) 
    {
        intervalSeconds.store (jmax (newIntervalSeconds, 0.0));

        if (! isThreadRunning())
            startThread();
    }

    /** */
    void startTimer (int intervalInMilliseconds) 
    {
        startTimer (RelativeTime::milliseconds (intervalInMilliseconds).inSeconds());
    }

    /** */
    void startTimerHz (int timerFrequencyHz) 
    {
        startTimer (1000.0 / static_cast<double> (timerFrequencyHz) / 1000.0);
    }

    /** */
    void stopTimer()
    {
        shutdownThreadSafely (*this);
    }

    /** */
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
JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wall",
                                     "-Wconversion",
                                     "-Wimplicit-const-int-float-conversion",
                                     "-Wsign-conversion")

namespace details
{
    namespace IsSTLContainerImpl
    {
        template <typename T>       struct is_stl_container                                     : std::false_type {};
        template <typename T, std::size_t N> struct is_stl_container<std::array<T, N>>          : std::true_type {};
        template <typename... Args> struct is_stl_container<std::vector<Args...>>               : std::true_type {};
        template <typename... Args> struct is_stl_container<std::deque<Args...>>                : std::true_type {};
        template <typename... Args> struct is_stl_container<std::list<Args...>>                 : std::true_type {};
        template <typename... Args> struct is_stl_container<std::forward_list<Args...>>         : std::true_type {};
        template <typename... Args> struct is_stl_container<std::set<Args...>>                  : std::true_type {};
        template <typename... Args> struct is_stl_container<std::multiset<Args...>>             : std::true_type {};
        template <typename... Args> struct is_stl_container<std::map<Args...>>                  : std::true_type {};
        template <typename... Args> struct is_stl_container<std::multimap<Args...>>             : std::true_type {};
        template <typename... Args> struct is_stl_container<std::unordered_set<Args...>>        : std::true_type {};
        template <typename... Args> struct is_stl_container<std::unordered_multiset<Args...>>   : std::true_type {};
        template <typename... Args> struct is_stl_container<std::unordered_map<Args...>>        : std::true_type {};
        template <typename... Args> struct is_stl_container<std::unordered_multimap<Args...>>   : std::true_type {};
        template <typename... Args> struct is_stl_container<std::stack<Args...>>                : std::true_type {};
        template <typename... Args> struct is_stl_container<std::queue<Args...>>                : std::true_type {};
        template <typename... Args> struct is_stl_container<std::priority_queue<Args...>>       : std::true_type {};
    }

    template <typename T>
    struct IsSTLContainer
    {
        static constexpr bool const value = IsSTLContainerImpl::is_stl_container<std::decay_t<T>>::value;
    };
}

/** */
template<class Container,
         typename IndexType,
         details::IsSTLContainer<Container>::value = true>
inline bool moveItem (Container& v, IndexType oldIndex, IndexType newIndex)
{
    if (oldIndex > v.size() || newIndex > v.size())
    {
        jassertfalse;
        return false;
    }

    if (oldIndex > newIndex)
    {
        const auto containerREnd = std::crend (v);
        std::rotate (containerREnd - oldIndex - 1,
                     containerREnd - oldIndex,
                     containerREnd - newIndex);
    }
    else        
    {
        const auto containerBegin = std::cbegin (v);
        std::rotate (containerBegin + oldIndex,
                     containerBegin + oldIndex + 1,
                     containerBegin + newIndex + 1);
    }

    return true;
}

/** */
template<class Container, 
         typename IndexType>
inline bool moveItemToBack (Container& v, IndexType itemIndex)
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
template<class Container, 
         typename IndexType>
inline bool moveItemToFront (Container& v, IndexType itemIndex)
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
template<class Container, 
         typename IndexType,
         details::IsSTLContainer<Container>::value = true>
inline bool removeItem (Container& v, IndexType itemIndex)
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
template<class Container, 
         typename IndexType,
         typename ItemType,
         details::IsSTLContainer<Container>::value = true>
inline bool replaceItem (Container& v, IndexType itemIndex, ItemType& newItem)
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
template<class Container, 
         typename Value,
         details::IsSTLContainer<Container>::value = true>
inline void append (Container& destination, const Container& source)
{
    destination.insert (std::end (destination), std::begin (source), std::end (source));
}

/** */
template<class Container, 
         typename Value,
         typename Predicate,
         details::IsSTLContainer<Container>::value = true>
inline Value removeAndReturn (Container& container, Predicate predicate)
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
template<class Container, 
         typename Value,
         details::IsSTLContainer<Container>::value = true>
inline void removeAllInstancesOf (Container& container, const Value& value)
{
    container.erase (std::remove (container.begin(), container.end(), value), container.cend());
}

/** */
template<class Container, 
         typename Value,
         details::IsSTLContainer<Container>::value = true>
inline bool contains (const Container& container, const Value& value)
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
/** @returns the operating system's maximum length of a path. */
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

//==============================================================================
/** @returns a unique hash representing the user's system. */
inline [[deprecated ("Please use juce::SystemStats::getUniqueDeviceID() instead.")]] String createSystemHash()
{
    MemoryBlock data;

    {
        Array<File> roots;
        File::findFileSystemRoots (roots);

        MemoryOutputStream mos (data, false);
        for (const auto& r : roots)
            if (! (r.isOnRemovableDrive() || r.isOnCDRomDrive() || r.isSymbolicLink()))
                mos << r.hashCode64();

        mos
            << SystemStats::getLogonName()
            << SystemStats::getFullUserName()
            << SystemStats::getComputerName();
    }

    return SHA256 (data).toHexString();
}
