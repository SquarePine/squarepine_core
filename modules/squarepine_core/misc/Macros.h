/** This section removes stupid min/max macros to avoid clashing
    with STL min/max implementations.

    Too many idiot coders enjoy redefining the min/max macros in their libraries...
    And someone behind the STL implementation at MSFT decided it was a good idea
    to provide inane (and ugly) versions of them too! (hashtag YOLO and IHEARTRECREATINGMINMAXBRO)

    This section of code should help avoid clashing of sorts,
    and clearing up the already zany global namespace.

    Give me a break - use jmin() and jmax() instead!
    JUCE is equally at fault here, with its jmin() and jmax() functions.
*/

#undef NOMINMAX // Avoids additional clashing
#define NOMINMAX 1

#undef min
#undef max
#undef __min
#undef __max
#undef MIN
#undef MAX
#undef fmin
#undef fminf
#undef fminl
#undef fmax
#undef fmaxf
#undef fmaxl

//==============================================================================
#if DOXYGEN || JUCE_WINDOWS || JUCE_LINUX || JUCE_MAC || JUCE_BSD || JUCE_WASM
    #define SQUAREPINE_IS_DESKTOP      1 //< Currently running on a desktop.
#elif JUCE_ANDROID || JUCE_IPHONE || JUCE_IOS
    #define SQUAREPINE_IS_MOBILE       1 //< Currently running on a mobile device.
#else
    #error "What kind of operating system is this? Please fix the project platform format macro!"
#endif

//==============================================================================
#if JUCE_DEBUG
    /** Handy macro for outputting a pointer's address into the debug log. */
    #define DBG_PRINT_POINTER(object) \
        DBG ("\t- " + String (typeid (*object).name()) + \
             " (0x" + String::toHexString ((pointer_sized_int) object) + ")")
#else
    #define DBG_PRINT_POINTER(object) (void) object;
#endif

//==============================================================================
/** This can be used to create a 'makeCustomSmartPointerType' function. */
#define SQUAREPINE_CREATE_MAKE_PTR_TYPE(CustomSmartPointerType) \
    template<class Type, class... ConstructorInitList> \
    inline typename std::enable_if<! std::is_array<Type>::value, CustomSmartPointerType<Type>>::type make##CustomSmartPointerType (ConstructorInitList&&... args) \
    { \
        return CustomSmartPointerType<Type> (new Type (std::forward<ConstructorInitList> (args)...)); \
    }

/** This creates a 'make' function in conjunction with juce::ReferenceCountedObjectPtr. */
SQUAREPINE_CREATE_MAKE_PTR_TYPE (ReferenceCountedObjectPtr)

/** */
template<typename Type, typename... Args>
ReferenceCountedObjectPtr<Type> make_refptr (Args&&... args)
{
    return ReferenceCountedObjectPtr<Type> (new Type (std::forward<Args> (args)...));
}

//==============================================================================
#undef CREATE_UNIQUE_PTR_CAST_FUNC

/** */
#define CREATE_UNIQUE_PTR_CAST_FUNC(funcName, castType) \
    template<typename DestinationType, typename SourceType> \
    inline std::unique_ptr<DestinationType> funcName (std::unique_ptr<SourceType> source) \
    { \
        if (auto* dest = castType<DestinationType*> (source.get())) \
        { \
            source.release(); \
            return std::unique_ptr<DestinationType> (dest); \
        } \
        return {}; \
    }

/** */
CREATE_UNIQUE_PTR_CAST_FUNC (staticCastUniquePtr, static_cast)
/** */
CREATE_UNIQUE_PTR_CAST_FUNC (dynamicCastUniquePtr, dynamic_cast)
/** */
CREATE_UNIQUE_PTR_CAST_FUNC (constCastUniquePtr, const_cast)
/** */
CREATE_UNIQUE_PTR_CAST_FUNC (reinterpretCastUniquePtr, reinterpret_cast)

#undef CREATE_UNIQUE_PTR_CAST_FUNC
