/** A subclass of AudioProcessorParameter that provides an easy way to create a
    parameter which maps onto a given NormalisableRange.

    @see AudioParameterFloat
*/
class AudioParameterDouble : public AudioProcessorParameterWithID
{
public:
    /** Creates a AudioParameterFloat with an ID, name, and range.

        On creation, its value is set to the default value.
    */
    AudioParameterDouble (const String& parameterID, const String& name,
                          NormalisableRange<double> normalisableRange,
                          double defaultValue,
                          const String& label = String(),
                          Category category = AudioProcessorParameter::genericParameter);

    /** Creates a AudioParameterFloat with an ID, name, and range.

        On creation, its value is set to the default value.

        For control over skew factors, you can use the other
        constructor and provide a NormalisableRange.
    */
    AudioParameterDouble (const String& parameterID, const String& name,
                          double minValue,
                          double maxValue,
                          double defaultValue);

    /** @returns the parameter's current value. */
    double get() const noexcept                  { return value; }

    /** @returns the parameter's current value. */
    operator double() const noexcept             { return value; }

    /** Changes the parameter's current value. */
    AudioParameterDouble& operator= (double newValue);

    /** Provides access to the parameter's range. */
    NormalisableRange<double> range;

private:
    //==============================================================================
    double value, defaultValue;

    float getValue() const override;
    void setValue (float newValue) override;
    float getDefaultValue() const override;
    String getText (float, int) const override;
    float getValueForText (const String&) const override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioParameterDouble)
};
