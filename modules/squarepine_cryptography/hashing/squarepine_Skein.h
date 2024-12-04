/**

    @see https://en.wikipedia.org/wiki/Skein_(hash_function)
*/
class Skein final
{
public:
    Skein() = default;
    ~Skein() = default;

    //==============================================================================
    enum class Mode
    {
        rotation256,
        rotation512,
        rotation1024
    };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Skein)
};
