//==============================================================================
/** Add this to your class to give users access to shorcuts for writing
    std::shared_ptr and std::weak_ptr instances.
*/
#define SQUAREPINE_MAKE_SHAREABLE(ClassName) \
    using Ptr = std::shared_ptr<ClassName>; \
    using WeakPtr = std::weak_ptr<ClassName>;

//==============================================================================
/** Creates a class that cannot be instantiated.

    This is an effective alternative to a namespace.
*/
#define SQUAREPINE_DECLARE_TOOL_CLASS(ClassName) \
    JUCE_PREVENT_HEAP_ALLOCATION \
    JUCE_DECLARE_NON_COPYABLE (ClassName) \
    ClassName() = delete;

//==============================================================================
/** Prevents std::unique_ptr<> creation of the specified class.

    @warning You will need to call this in the global namespace for it to work!
*/
#define SQUAREPINE_PREVENT_UNIQUE_PTR(ClassName) \
    namespace std \
    { \
        template<> \
        class unique_ptr<ClassName> \
        { \
            unique_ptr() = delete; \
        }; \
    }

/** Prevents std::shared_ptr<> creation of the specified class.

    @warning You will need to call this in the global namespace for it to work!
*/
#define SQUAREPINE_PREVENT_SHARED_PTR(ClassName) \
    namespace std \
    { \
        template<> \
        class shared_ptr<ClassName> \
        { \
            shared_ptr() = delete; \
        }; \
    }
