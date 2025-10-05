/** Base-class for designing a dynamically loop-unrolled version of array element iterating.

    Simply derive from this class, and implement performAtIndex() with
    whatever array whose items you would like to iterate through and do
    something with!

    It's up to your subclass to reference an array of some kind;
    this is meant to help you iterate through elements in chunks only
    thus simply doing the boiler-plate work for you.
*/
class ArrayIterationUnroller
{
public:
    /** Constructor.

        @param arraySize The number of elements.
    */
    ArrayIterationUnroller (int arraySize = 0);

    /** Destructor. */
    virtual ~ArrayIterationUnroller() = default;

    //==============================================================================
    /** Call this to begin performing whatever it is you need in your subclass!

        This will simply try to minimise the amount of run-throughs
        by iterating through ranges of blocks of indices in one fell swoop.
    */
    void run();

protected:
    //==============================================================================
    /** Changes the size of the unroller.
        Don't forget to do this if you set the initial size to 0 in the constructor!
    */
    void resize (int newSize);

    /** Perform whatever task it is you need to be done quickly in this method.
        You should always attempt doing so in the quickest fashion
        possible for better performance!
    */
    virtual void performAtIndex (int index) = 0;

private:
    //==============================================================================
    int sizeOfArray = 0;

    //==============================================================================
    /** Internal tool class for iterating through a single block,
        starting at a specified offset.
    */
    class InternalPerform;
    friend class InternalPerform;

    //==============================================================================
    void pairedPerform (int index);
    void quadPerform (int index);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArrayIterationUnroller)
};
