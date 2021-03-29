AudioParameterDouble::AudioParameterDouble (const String& idToUse, const String& nameToUse,
                                            NormalisableRange<double> r, double def,
                                            const String& labelToUse, Category categoryToUse) :
    AudioProcessorParameterWithID (idToUse, nameToUse, labelToUse, categoryToUse),
    range (r),
    value (def),
    defaultValue (def)
{
}

AudioParameterDouble::AudioParameterDouble (const String& pid, const String& nm, double minValue, double maxValue, double def) :
    AudioProcessorParameterWithID (pid, nm),
    range (minValue, maxValue),
    value (def),
    defaultValue (def)
{
}

float AudioParameterDouble::getValue() const                            { return (float) range.convertTo0to1 (value); }
void AudioParameterDouble::setValue (float newValue)                    { value = range.convertFrom0to1 ((double) newValue); }
float AudioParameterDouble::getDefaultValue() const                     { return (float) range.convertTo0to1 ((double) defaultValue); }
float AudioParameterDouble::getValueForText (const String& text) const  { return (float) range.convertTo0to1 (text.getDoubleValue()); }

String AudioParameterDouble::getText (float v, int length) const
{
    const String asText (range.convertFrom0to1 ((double) v), 2);
    return length > 0 ? asText.substring (0, length) : asText;
}

AudioParameterDouble& AudioParameterDouble::operator= (double newValue)
{
    if (value != newValue)
        setValueNotifyingHost ((float) range.convertTo0to1 (newValue));

    return *this;
}
