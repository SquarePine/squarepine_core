//==============================================================================
class ArrayIterationUnroller::InternalPerform final
{
public:
    InternalPerform (ArrayIterationUnroller& o) :
        owner (o)
    {
        jumpTable[0] = &InternalPerform::singles;
        jumpTable[1] = &InternalPerform::pairs;
        jumpTable[2] = &InternalPerform::triples;
        jumpTable[3] = &InternalPerform::quads;
        jumpTable[4] = &InternalPerform::quints;
        jumpTable[5] = &InternalPerform::sexts;
        jumpTable[6] = &InternalPerform::septs;
        jumpTable[7] = &InternalPerform::octs;
        jumpTable[8] = &InternalPerform::nonuples;
        jumpTable[9] = &InternalPerform::decas;
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
    /** This will iterate through the array with a jump table */
    void runBranched (int index, int offset)
    {
        (this->*jumpTable[index]) (offset);
    }

    //==============================================================================
    /** This will perform an iteration with the specified increment.

        Be sure to specify the correct method to the increment!

        @param[out] remainder   The remainder of elements that need to be iterated through.
        @param increment        The amount of items to process and therefore to increment by.
    */
    template <void (InternalPerform::*unrollMethod) (int index)>
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

    //==============================================================================
    /** Typedef for creating a member-function-pointer array. */
    typedef void (InternalPerform::*MethodPointer) (int);

    MethodPointer jumpTable[10]; // NB: Do NOT make this static - this will not work in a multi-threaded environment!

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InternalPerform)
};

//==============================================================================
ArrayIterationUnroller::ArrayIterationUnroller (int arraySize) :
    sizeOfArray (arraySize)
{
    jassert (sizeOfArray >= 0);
}

ArrayIterationUnroller::~ArrayIterationUnroller()
{
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

   #if SQUAREPINE_ARRAY_ITERATION_UNROLLER_CHECK_BIG_NUMS
    while (remainder >= 100)
        perf.callMethodWithIncrement<&InternalPerform::hundred> (remainder, 100);
   #endif

    while (remainder >= 20)
        perf.callMethodWithIncrement<&InternalPerform::twenty> (remainder, 20);

    if (remainder >= 10)
        perf.callMethodWithIncrement<&InternalPerform::decas> (remainder, 10);

    if (remainder > 0)
        perf.runBranched (remainder - 1, sizeOfArray - remainder);
   #endif
}
