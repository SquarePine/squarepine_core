/** A simple allocator which has a single piece of memory that will be used to
    give unique pointers for objects to initialise to.

    @note There is no functionality for clearing single objects from the chunk,
          only clearing the entire chunk is implemented.
*/
template<size_t sizeBytes = 1024 * 1024 * 1 /* 1 MB */, size_t alignmentBytes = 4>
class Allocator final{
public:
    //==============================================================================
    /** Constructor, which allocates an internal heap to use as a memory pool.

        If for any reason this fails to allocate, it will just leave the internal heap with a null pointer;
        of course this is assuming that the system's malloc() function doesn't throw.
    */
    Allocator() :
        base (sizeBytes, true),
        marker (base)
    {
    }

    /** Destructor. */
    ~Allocator()
    {
        masterReference.clear();
    }

    //==============================================================================
    /** @returns the total size of the allocator's heap.
        By default, this is 1 megabyte, which you can configure on template construction.
    */
    constexpr size_t getSizeBtes() const noexcept       { return sizeBytes; }

    /** @returns the byte of alignment of the allocations.
        By default, this is 4 bytes, which you can configure on template construction.
    */
    constexpr size_t getAlignmentBytes() const noexcept { return alignmentBytes; }

    //==============================================================================
    /** @returns the start pointer position within the allocator's heap. */
    size_t getStartPosition() const noexcept            { return (size_t) base.getData(); }

    /** @returns the current pointer position within the allocator's heap. */
    size_t getCurrentPosition() const noexcept          { return (size_t) marker; }

    /** @returns the remaining space available for allocations. */
    size_t getRemainingSpace() const noexcept           { return getSize() - getCurrentPosition(); }

    //==============================================================================
    /** Manually allocate some number of bytes.

        @param numBytesToAllocate The number of bytes to allocate for an object.

        @returns An address where an object can be initialised using placement-new,
                 or nullptr if there is not enough memory left.
    */
    void* allocate (size_t numBytesToAllocate) const
    {
        // Determine an allocation size that will align to our requested byte alignment:
        const auto remainder = static_cast<size_t> (numBytesToAllocate % alignmentBytes);
        auto allocationSize = numBytesToAllocate;

        if (remainder != 0)
            allocationSize += alignmentBytes - remainder;

        // Attempt allocating:
        if (isPositiveAndBelow (static_cast<size_t> (getCurrentPosition()) + allocationSize - getStartPosition(), sizeBytes))
        {
            auto* const newAddress = (void*) marker;
            marker += allocationSize;
            return newAddress;
        }

        Logger::writeToLog ("Allocator: ran out of memory!");
        jassertfalse;
        return nullptr;
    }

private:
    //==============================================================================
    /** */
    class ObjectBase
    {
    public:
        /** */
        ObjectBase() noexcept = default;

        /** */
        virtual ~ObjectBase() noexcept = default;

        //==============================================================================
        /** */
        virtual void reset() = 0;

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE (ObjectBase)
    };

public:
    //==============================================================================
    /** */
    template<typename SourceType>
    class Object final : public ObjectBase
    {
    public:
        //==============================================================================
        /** */
        using Type = SourceType;

        //==============================================================================
        /** */
        Object() noexcept = default;

        /** */
        Object (Object&& other) noexcept :
            value (std::move (other.value)),
            parent (std::move (other.parent))
        {
        }

        /** */
        ~Object() override                      { reset(); }

        //==============================================================================
        /** @returns */
        constexpr bool isNull() const noexcept  { return value == nullptr; }
        /** @returns */
        constexpr bool isValid() const noexcept { return ! isNull(); }

        /** @returns */
        Type& operator&()                       { jassert (isValid()); return *value; }
        /** @returns */
        Type& operator*()                       { jassert (isValid()); return *value; }

        //==============================================================================
        /** */
        void reset() override
        {
            if (value != nullptr)
                value->~Type();

            // if (parent != nullptr)
                // parent->clear (value);

            value = nullptr;
            parent = {};
        }

    private:
        //==============================================================================
        friend class Allocator;

        Type* value = nullptr;
        WeakReference<Allocator> parent;

        //==============================================================================
        constexpr size_t getStart() const noexcept  { return value; }
        constexpr size_t getEnd() const noexcept    { return getStart() + sizeof (Type); }

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE (Object)
    };

    //==============================================================================
    /** Allocates an object.

        @returns A new object placed within the allocator,
                 or nullptr if there wasn't enough space to put it.
    */
    template<typename ObjectType,
             typename... Args,
             typename Type = typename std::remove_cv_t<ObjectType>>
    Object<Type> allocateObject (Args... constructorArgs)
    {
        Type* v = nullptr;

        if (auto* address = allocate (sizeof (Type)))
            v = new (address) Type (constructorArgs...);

        if (v == nullptr)
            return {};

        Object<Type> o;
        o.value = v;
        o.parent = this;
        return o;
    }

    //==============================================================================
#if 0
    /** Allocates an object.

        @returns A new object placed within the allocator,
                 or nullptr if there wasn't enough space to put it.
    */
    template<typename ObjectType,
             typename... Args,
             typename Type = typename std::remove_cv<ObjectType>::type>
    Type* allocateObject (Args... constructorArgs) const
    {
        if (auto* address = allocate (sizeof (Type)))
            return new (address) Type (constructorArgs...);

        return nullptr;
    }

    /** @returns a newly allocated int8_t. */
    int8_t* allocateObject (int8_t value) const             { return allocateObject<int8_t> (value); }
    /** @returns a newly allocated int16_t. */
    int16_t* allocateObject (int16_t value) const           { return allocateObject<int16_t> (value); }
    /** @returns a newly allocated int32_t. */
    int32_t* allocateObject (int32_t value) const           { return allocateObject<int32_t> (value); }
    /** @returns a newly allocated int64_t. */
    int64_t* allocateObject (int64_t value) const           { return allocateObject<int64_t> (value); }
    /** @returns a newly allocated uint8_t. */
    uint8_t* allocateObject (uint8_t value) const           { return allocateObject<uint8_t> (value); }
    /** @returns a newly allocated uint16_t. */
    uint16_t* allocateObject (uint16_t value) const         { return allocateObject<uint16_t> (value); }
    /** @returns a newly allocated uint32_t. */
    uint32_t* allocateObject (uint32_t value) const         { return allocateObject<uint32_t> (value); }
    /** @returns a newly allocated uint64_t. */
    uint64_t* allocateObject (uint64_t value) const         { return allocateObject<uint64_t> (value); }
    /** @returns a newly allocated bool. */
    bool* allocateObject (bool value) const                 { return allocateObject<bool> (value); }
    /** @returns a newly allocated float. */
    float* allocateObject (float value) const               { return allocateObject<float> (value); }
    /** @returns a newly allocated double. */
    double* allocateObject (double value) const             { return allocateObject<double> (value); }
    /** @returns a newly allocated long double. */
    long double* allocateObject (long double value) const   { return allocateObject<long double> (value); }

    //==============================================================================
    /** Attempt creating a copy of a specified object whilst allocating it
        within the instance of this class.

        @note This will fail at compile-time if the object passed in does not have a copy-constructor!

        @returns A copy of the object. If there is not enough memory left, nullptr is returned.
    */
    template<typename SourceType, typename Type = typename std::remove_cv<SourceType>::type>
    Type* createCopy (SourceType& object) const
    {
        if (auto* const address = allocateObject<Type>())
            return new (address) Type (object);

        return nullptr;
    }

    /** Attempt creating a copy of a specified object whilst allocating it
        within the instance of this class.

        @note This will fail at compile-time if the object passed in does not have a copy-constructor!

        @returns A copy of the object. If there is not enough memory left, nullptr is returned.
    */
    template<typename SourceType, typename Type = typename std::remove_cv<SourceType>::type>
    Type* createCopy (SourceType* object) const
    {
        if (object != nullptr)
            return createCopy<Type> (*object);

        jassertfalse; //Not sure how you messed this up!
        return nullptr;
    }

    /** Attempt creating a copy of a specified object whilst allocating it
        within the instance of this class.

        @note This will fail at compile-time if the object passed in does not have a move constructor!

        @returns A copy of the object. If there is not enough memory left, nullptr is returned.
    */
    template<typename SourceType, typename Type = typename std::remove_cv<SourceType>::type>
    Type* createCopy (SourceType&& object) const
    {
        if (auto* const address = allocateObject<Type>())
            return new (address) Type (object);

        return nullptr;
    }

#endif

    //==============================================================================
    /** Resets the marker to the base memory location of the allocated memory. */
    void reset (bool clearMemory = false)
    {
        if (clearMemory)
            base.clear (sizeBytes);

        occupiedSpaces.clear();
        marker = base.getData(); // Reset the marker position to the beginning of the data set.
    }

private:
    //==============================================================================
    HeapBlock<uint8, false> base;
    Array<Range<int64>> occupiedSpaces;
    mutable uint8* marker = nullptr;

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (Allocator)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Allocator)
};
