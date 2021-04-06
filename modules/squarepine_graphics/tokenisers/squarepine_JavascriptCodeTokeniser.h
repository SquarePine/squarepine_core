/** A simple lexical analyser for syntax colouring of Javascript code. */
class JavascriptTokeniser final : public CodeTokeniser
{
public:
    /** */
    JavascriptTokeniser() = default;

    //==============================================================================
    /** */
    static CodeEditorComponent::ColourScheme getDefaultEditorColourScheme();

    //==============================================================================
    /** */
    CodeEditorComponent::ColourScheme getDefaultColourScheme() override;
    /** */
    int readNextToken (CodeDocument::Iterator& source) override;

    //==============================================================================
    /** */
    enum TokenType
    {
        tokenType_error = 0,
        tokenType_comment,
        tokenType_keyword,
        tokenType_internalClass,
        tokenType_operator,
        tokenType_identifier,
        tokenType_integer,
        tokenType_float,
        tokenType_string,
        tokenType_bracket,
        tokenType_punctuation
    };

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JavascriptTokeniser)
};

