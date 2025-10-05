//==============================================================================
class ArrayIterationUnroller::InternalPerform final
{
public:
    InternalPerform (ArrayIterationUnroller& o) :
        owner (o)
    {
        jumpTable =
        {
            std::bind (&InternalPerform::singles, this, std::placeholders::_1),
            std::bind (&InternalPerform::pairs, this, std::placeholders::_1),
            std::bind (&InternalPerform::triples, this, std::placeholders::_1),
            std::bind (&InternalPerform::quads, this, std::placeholders::_1),
            std::bind (&InternalPerform::quints, this, std::placeholders::_1),
            std::bind (&InternalPerform::sexts, this, std::placeholders::_1),
            std::bind (&InternalPerform::septs, this, std::placeholders::_1),
            std::bind (&InternalPerform::octs, this, std::placeholders::_1),
            std::bind (&InternalPerform::nonuples, this, std::placeholders::_1),
            std::bind (&InternalPerform::decas, this, std::placeholders::_1)
        };
    }

    //==============================================================================
    void singles (int offset)
    {
        owner.performAtIndex (offset);
    }

    void pairs (int offset)
    {
        owner.pairedPerform (offset);
    }

    void triples (int offset)
    {
        owner.pairedPerform (offset);
        owner.performAtIndex (offset + 2);
    }

    void quads (int offset)
    {
        owner.quadPerform (offset);
    }

    void quints (int offset)
    {
        owner.quadPerform (offset);
        owner.performAtIndex (offset + 4);
    }

    void sexts (int offset)
    {
        owner.quadPerform (offset);
        owner.pairedPerform (offset + 4);
    }

    void septs (int offset)
    {
        owner.quadPerform (offset);
        owner.pairedPerform (offset + 4);
        owner.performAtIndex (offset + 6);
    }

    void octs (int offset)
    {
        owner.quadPerform (offset);
        owner.quadPerform (offset + 4);
    }

    void nonuples (int offset)
    {
        owner.quadPerform (offset);
        owner.quadPerform (offset + 4);
        owner.performAtIndex (offset + 8);
    }

    void decas (int offset)
    {
        owner.quadPerform (offset);
        owner.quadPerform (offset + 4);
        owner.pairedPerform (offset + 8);
    }

    void twenty (int offset)
    {
        decas (offset);
        decas (offset + 10);
    }

    void hundred (int offset)
    {
        twenty (offset);
        twenty (offset + 20);
        twenty (offset + 40);
        twenty (offset + 60);
        twenty (offset + 80);
    }

    //==============================================================================
    /** Typedef for creating a member-function-pointer array. */
    using MethodPointer = std::function<void (int)>;

    /** This will perform an iteration with the specified increment.

        Be sure to specify the correct method to the increment!

        @param[out] remainder   The remainder of elements that need to be iterated through.
        @param increment        The amount of items to process and therefore to increment by.
    */
    template <auto unrollMethod>
    void callMethodWithIncrement (int& remainder, int increment)
    {
        // Find out how many blocks of "increment" elements we can iterate through:
        const auto numBlocks = (int) std::floor ((double) owner.sizeOfArray / (double) increment);

        // Iterate through the major chunks (in incremental order, in case that's necessary):
        for (int i = 0; i < numBlocks; ++i)
            (this->*unrollMethod) (i * increment);

        remainder = owner.sizeOfArray - (numBlocks * increment);
    }

private:
    //==============================================================================
    ArrayIterationUnroller& owner;
    std::array<MethodPointer, 10> jumpTable;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InternalPerform)
};

//==============================================================================
ArrayIterationUnroller::ArrayIterationUnroller (int arraySize) :
    sizeOfArray (arraySize)
{
    jassert (sizeOfArray >= 0);
}

//==============================================================================
void ArrayIterationUnroller::resize (int newSize)
{
    sizeOfArray = std::max (newSize, 0);
}

void ArrayIterationUnroller::pairedPerform (int index)
{
    performAtIndex (index);
    performAtIndex (index + 1);
}

void ArrayIterationUnroller::quadPerform (int index)
{
    pairedPerform (index);
    pairedPerform (index + 2);
}

//==============================================================================
void ArrayIterationUnroller::run()
{
   #if SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR
    // Linear iteration:
    for (int i = 0; i < sizeOfArray; ++i)
        performAtIndex (i);
   #else
    if (sizeOfArray <= 0)
        return;

    InternalPerform perf (*this);

    auto remainder = sizeOfArray;

    while (remainder >= 100)
        perf.callMethodWithIncrement<&InternalPerform::hundred> (remainder, 100);

    while (remainder >= 20)
        perf.callMethodWithIncrement<&InternalPerform::twenty> (remainder, 20);

    while (remainder >= 10)
        perf.callMethodWithIncrement<&InternalPerform::decas> (remainder, 10);

    while (remainder >= 2)
        perf.callMethodWithIncrement<&InternalPerform::pairs> (remainder, 2);

    while (remainder > 0)
        perf.callMethodWithIncrement<&InternalPerform::singles> (remainder, 1);
   #endif
}
