/** Simple cumulative average class which you can add values to and will return the mean of them.

    This can be used when you don't know the total number of values that need to be averaged.
 */
template<typename ValueType, typename IndexType = int>
class MovingAccumulator final
{
public:
    //==============================================================================
    /** */
    using Type = ValueType;

    //==============================================================================
    /** Creates an empty MovingAccumulator. */
    MovingAccumulator() noexcept = default;

    /** Creates a copy of another MovingAccumulator. */
    MovingAccumulator (const MovingAccumulator& other) noexcept = default;

    /** Copies another MovingAccumulator into this one. */
    MovingAccumulator& operator= (const MovingAccumulator& other) noexcept = default;

    /** Destructor. */
    ~MovingAccumulator() noexcept = default;

    //==============================================================================
    using AccumulationFunction = std::function<Type (Type current, Type newValue, IndexType numValues)>;

    AccumulationFunction function;

    //==============================================================================
    /** Resets the MovingAccumulator. */
    void reset() noexcept
    {
        current = static_cast<Type> (0);
        numValues = static_cast<IndexType> (0);
    }

    /** @returns the result after adding a new value to contribute to the accumulation. */
    Type add (Type newValue) noexcept
    {
        if (function != nullptr)
        {
            current = (newValue + (numValues * current)) / (numValues + 1);
            ++numValues;
        }

        return current;
    }

    /** @returns the current result. */
    Type get() const noexcept { return current; }

    /** @returns the number of values that have contributed to the current result. */
    IndexType getNumValues() const noexcept { return numValues; }

private:
    //==============================================================================
    Type current = static_cast<Type> (0);
    IndexType numValues = static_cast<IndexType> (0);

    //==============================================================================
    JUCE_LEAK_DETECTOR (MovingAccumulator)
};
