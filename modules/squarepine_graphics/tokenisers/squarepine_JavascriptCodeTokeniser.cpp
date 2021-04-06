//==============================================================================
namespace JsTokeniserFunctions
{
    inline bool isIdentifierStart (juce_wchar c) noexcept
    {
        return CharacterFunctions::isLetter (c) || c == '_' || c == '$';
    }

    inline bool isIdentifierBody (juce_wchar c) noexcept
    {
        return CharacterFunctions::isLetterOrDigit (c) || c == '_' || c == '$';
    }

    enum
    {
        smallestKeywordSize = 2,
        largestKeywordSize = 13     //"XMLHttpRequest" is presently the longest keyword.
    };

    using CharPointerType = String::CharPointerType;

    inline int isReservedKeyword (const CharPointerType& token, int tokenLength) noexcept
    {
        if (tokenLength < smallestKeywordSize || tokenLength > largestKeywordSize)
            return false;

        // Keyword lookup:
        #undef SP_CHECK_IF_TOKEN_MATCHES
        #define SP_CHECK_IF_TOKEN_MATCHES(name, str) \
            if (CharacterFunctions::compare (token, CharPointer_ASCII (str)) == 0) \
                return JavascriptTokeniser::tokenType_keyword;

        SP_JS_KEYWORDS (SP_CHECK_IF_TOKEN_MATCHES)
        #undef SP_CHECK_IF_TOKEN_MATCHES

        // Class name lookup:
        #undef SP_LIST_OF_JS_CLASSES
        #undef SP_CHECK_IF_TOKEN_MATCHES
        #define SP_CHECK_IF_TOKEN_MATCHES(JSClass) \
            if (CharacterFunctions::compare (token, JSClass::getClassName().getCharPointer()) == 0) \
                return JavascriptTokeniser::tokenType_internalClass;

        #define SP_LIST_OF_JS_CLASSES(X) \
            X (ArrayBufferClass)    X (ArrayClass)      X (AtomicsClass)    X (BigIntClass)     X (BooleanClass) \
            X (ConsoleClass)        X (DataViewClass)   X (DateClass)       X (JSONClass)       X (JUCEClass) \
            X (MapClass)            X (MathClass)       X (NumberClass)     X (ObjectClass)     X (ProxyClass)      X (RegExpClass) \
            X (SetClass)            X (StringClass)     X (SymbolClass)     X (WeakMapClass)    X (WeakSetClass)    X (XMLHttpRequestClass)

        SP_LIST_OF_JS_CLASSES (SP_CHECK_IF_TOKEN_MATCHES)
        #undef SP_LIST_OF_JS_CLASSES
        #undef SP_CHECK_IF_TOKEN_MATCHES

        return -1;
    }

    template<typename Iterator>
    inline int parseIdentifier (Iterator& source) noexcept
    {
        int tokenLength = 0;
        CharPointerType::CharType possibleIdentifier[100];
        CharPointerType possible (possibleIdentifier);

        while (isIdentifierBody (source.peekNextChar()))
        {
            const auto c = source.nextChar();

            if (tokenLength++ < largestKeywordSize)
                possible.write (c);
        }

        if (tokenLength >= 2 && tokenLength <= largestKeywordSize)
        {
            possible.writeNull();

            const auto type = isReservedKeyword (CharPointerType (possibleIdentifier), tokenLength);

            if (type >= 0)
                return type;
        }

        return JavascriptTokeniser::tokenType_identifier;
    }
}

//==============================================================================
CodeEditorComponent::ColourScheme JavascriptTokeniser::getDefaultEditorColourScheme()
{
    static const CodeEditorComponent::ColourScheme::TokenType types[] =
    {
        { "Error",          Colours::red },
        { "Comment",        Colours::green },
        { "Keyword",        Colours::blue },
        { "InternalClass",  Colours::blue.withAlpha (0.9f) },
        { "Operator",       Colours::grey },
        { "Identifier",     Colours::black },
        { "Integer",        Colours::darkgrey },
        { "Float",          Colours::darkgrey },
        { "String",         Colours::darkred },
        { "Bracket",        Colours::black },
        { "Punctuation",    Colours::black }
    };

    CodeEditorComponent::ColourScheme cs;

    for (const auto& c : types)
        cs.set (c.name, c.colour);

    return cs;
}

CodeEditorComponent::ColourScheme JavascriptTokeniser::getDefaultColourScheme()
{
    return getDefaultEditorColourScheme();
}

int JavascriptTokeniser::readNextToken (CodeDocument::Iterator& source)
{
    source.skipWhitespace();

    const auto firstChar = source.peekNextChar();

    switch (firstChar)
    {
        case 0:
        break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.':
        {
            const auto result = CppTokeniserFunctions::parseNumber (source);

            if (result == tokenType_error)
            {
                source.skip();

                if (firstChar == '.')
                    return tokenType_punctuation;
            }

            return result;
        }

        case ',':
        case ';':
        case ':':
            source.skip();
            return tokenType_punctuation;

        case '(': case ')':
        case '{': case '}':
        case '[': case ']':
            source.skip();
            return tokenType_bracket;

        case '"':
        case '\'':
            CppTokeniserFunctions::skipQuotedString (source);
            return tokenType_string;

        case '+':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, '+', '=');
            return tokenType_operator;

        case '-':
        {
            source.skip();
            const auto result = CppTokeniserFunctions::parseNumber (source);

            if (result == tokenType_error)
            {
                CppTokeniserFunctions::skipIfNextCharMatches (source, '-', '=');
                return tokenType_operator;
            }

            return result;
        }

        case '*':
        case '%':
        case '=':
        case '!':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
            CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
            return tokenType_operator;

        case '/':
        {
            source.skip();

            if (source.peekNextChar() == '/')
            {
                source.skipToEndOfLine();
                return tokenType_comment;
            }

            if (source.peekNextChar() == '*')
            {
                source.skip();
                CppTokeniserFunctions::skipComment (source);
                return tokenType_comment;
            }

            if (source.peekNextChar() == '=')
                source.skip();

            return tokenType_operator;
        }

        case '<': case '>':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, firstChar);
            CppTokeniserFunctions::skipIfNextCharMatches (source, firstChar);
            CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
            return tokenType_operator;

        case '|':
        case '&':
        case '^':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, firstChar);
            CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
            return tokenType_operator;

        case '~':
        case '?':
            source.skip();
            return tokenType_operator;

        default:
            if (JsTokeniserFunctions::isIdentifierStart (firstChar))
                return JsTokeniserFunctions::parseIdentifier (source);

            source.skip();
        break;
    }

    return tokenType_error;
}
