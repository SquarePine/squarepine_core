//==============================================================================
class ArrayIterationUnroller::InternalPerform final
{
public:
    InternalPerform (ArrayIterationUnroller& o) :
        owner (o)
    {
        /*jumpTable[0] = &InternalPerform::singles;
        jumpTable[1] = &InternalPerform::pairs;
        jumpTable[2] = &InternalPerform::triples;
        jumpTable[3] = &InternalPerform::quads;
        jumpTable[4] = &InternalPerform::quints;
        jumpTable[5] = &InternalPerform::sexts;
        jumpTable[6] = &InternalPerform::septs;
        jumpTable[7] = &InternalPerform::octs;
        jumpTable[8] = &InternalPerform::nonuples;
        jumpTable[9] = &InternalPerform::decas;*/
    }

    //==============================================================================
    void singles (int offset, bool& needsToBreak)
    {
        owner.performAtIndex (offset, needsToBreak);
    }

    void pairs (int offset, bool& needsToBreak)
    {
        owner.pairedPerform (offset, needsToBreak);
    }

    void triples (int offset, bool& needsToBreak)
    {
        owner.pairedPerform (offset, needsToBreak);
        owner.performAtIndex (offset + 2, needsToBreak);
    }

    void quads (int offset, bool& needsToBreak)
    {
        owner.quadPerform (offset, needsToBreak);
    }

    void quints (int offset, bool& needsToBreak)
    {
        owner.quadPerform (offset, needsToBreak);
        owner.performAtIndex (offset + 4, needsToBreak);
    }

    void sexts (int offset, bool& needsToBreak)
    {
        owner.quadPerform (offset, needsToBreak);
        owner.pairedPerform (offset + 4, needsToBreak);
    }

    void septs (int offset, bool& needsToBreak)
    {
        owner.quadPerform (offset, needsToBreak);
        owner.pairedPerform (offset + 4, needsToBreak);
        owner.performAtIndex (offset + 6, needsToBreak);
    }

    void octs (int offset, bool& needsToBreak)
    {
        owner.quadPerform (offset, needsToBreak);
        owner.quadPerform (offset + 4, needsToBreak);
    }

    void nonuples (int offset, bool& needsToBreak)
    {
        owner.quadPerform (offset, needsToBreak);
        owner.quadPerform (offset + 4, needsToBreak);
        owner.performAtIndex (offset + 8, needsToBreak);
    }

    void decas (int offset, bool& needsToBreak)
    {
        owner.quadPerform (offset, needsToBreak);
        owner.quadPerform (offset + 4, needsToBreak);
        owner.pairedPerform (offset + 8, needsToBreak);
    }

    void twenty (int offset, bool& needsToBreak)
    {
        decas (offset, needsToBreak);
        decas (offset + 10, needsToBreak);
    }

    void hundred (int offset, bool& needsToBreak)
    {
        twenty (offset, needsToBreak);
        twenty (offset + 20, needsToBreak);
        twenty (offset + 40, needsToBreak);
        twenty (offset + 60, needsToBreak);
        twenty (offset + 80, needsToBreak);
    }

    //==============================================================================
    /** This will iterate through the array with a jump table */
    void runBranched (int index, int offset, bool& needsToBreak)
    {
        jumpTable[index] (offset, needsToBreak);
    }

    //==============================================================================
    /** This will perform an iteration with the specified increment.

        Be sure to specify the correct method to the increment!

        @param[out] remainder       The remainder of elements that need to be iterated through.
        @param increment            The amount of items to process and therefore to increment by.
        @param[out] needsToBreak    Will be true when needing to break out of the loop.
    */
    template <void (InternalPerform::*unrollMethod) (int index)>
    void callMethodWithIncrement (int& remainder, int increment, bool& needsToBreak)
    {
        // Find out how many blocks of "increment" elements we can iterate through:
        const auto numBlocks = (int) std::floor ((double) owner.sizeOfArray / (double) increment);

        // Iterate through the major chunks (in incremental order, in case that's necessary):
        for (int i = 0; i < numBlocks; ++i)
            (this->*unrollMethod) (i * increment, needsToBreak);

        remainder = owner.sizeOfArray - (numBlocks * increment);
    }

private:
    //==============================================================================
    ArrayIterationUnroller& owner;
    std::array<std::function<void (int, bool&)>, 10> jumpTable;

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
    sizeOfArray = jmax (newSize, 0);
}

void ArrayIterationUnroller::pairedPerform (int index, bool& needsToBreak)
{
    if (! needsToBreak) performAtIndex (index, needsToBreak);
    if (! needsToBreak) performAtIndex (index + 1, needsToBreak);
}

void ArrayIterationUnroller::quadPerform (int index, bool& needsToBreak)
{
    if (! needsToBreak) pairedPerform (index, needsToBreak);
    if (! needsToBreak) pairedPerform (index + 2, needsToBreak);
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

    bool needsToBreak = false;

   #if SQUAREPINE_ARRAY_ITERATION_UNROLLER_CHECK_BIG_NUMS
    while (remainder >= 100)
        perf.callMethodWithIncrement<&InternalPerform::hundred> (remainder, 100, needsToBreak);
   #endif

    while (remainder >= 20 && ! needsToBreak)
        perf.callMethodWithIncrement<&InternalPerform::twenty> (remainder, 20, needsToBreak);

    if (remainder >= 10 && ! needsToBreak)
        perf.callMethodWithIncrement<&InternalPerform::decas> (remainder, 10, needsToBreak);

    if (remainder > 0 && ! needsToBreak)
        perf.runBranched (remainder - 1, sizeOfArray - remainder, needsToBreak);
   #endif
}
