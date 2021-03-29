//==============================================================================
static const String flat = "b";
static auto oslash = String (CharPointer_UTF8 ("\xc3\xb8"));

//==============================================================================
Chord::Chord (Type t) noexcept :
    type (t)
{
}

Chord::Chord (Array<int> steps_, const String& symbol_) :
    type (Type::custom),
    steps (steps_),
    symbol (symbol_)
{
}

String Chord::toString() const
{
    auto res = String ((int) type) + "|" + symbol + "|";

    for (auto s : steps)
        res << String (s);

    return res;
}

Chord Chord::fromString (const String& s)
{
    auto tokens = StringArray::fromTokens (s, "|", "");
    if (tokens.size() >= 2)
    {
        Chord chord;
        chord.type = (Type) tokens[0].getIntValue();
        chord.symbol = tokens[1];

        for (int i = 2; i < tokens.size(); i++)
            if (tokens[i].isNotEmpty())
                chord.steps.add (tokens[i].getIntValue());

        return chord;
    }

    return Type::invalid;
}

Array<Chord::Type> Chord::getAllChordTypes()
{
    Array<Type> res;

    for (int i = (int) Type::majorTriad; i <= (int) Type::diminishedMinorNinth; ++i)
        res.add ((Type) i);

    return res;
}

String Chord::getName() const
{
    switch (type)
    {
        case Type::majorTriad:                  return TRANS ("Major Triad");
        case Type::minorTriad:                  return TRANS ("Minor Triad");
        case Type::diminishedTriad:             return TRANS ("Diminished Triad");
        case Type::augmentedTriad:              return TRANS ("Augmented Triad");
        case Type::majorSixth:                  return TRANS ("Major Sixth");
        case Type::minorSixth:                  return TRANS ("Minor Sixth");
        case Type::dominantSeventh:             return TRANS ("Dominant Seventh");
        case Type::majorSeventh:                return TRANS ("Major Seventh");
        case Type::minorSeventh:                return TRANS ("Minor Seventh");
        case Type::augmentedSeventh:            return TRANS ("Augmented Seventh");
        case Type::diminishedSeventh:           return TRANS ("Diminished Seventh");
        case Type::halfDiminishedSeventh:       return TRANS ("Half Diminished Seventh");
        case Type::minorMajorSeventh:           return TRANS ("Minor Major Seventh");
        case Type::power:                       return TRANS ("Power");
        case Type::suspendedSecond:             return TRANS ("Suspended Second");
        case Type::suspendedFourth:             return TRANS ("Suspended Fourth");
        case Type::majorNinth:                  return TRANS ("Major Ninth");
        case Type::dominantNinth:               return TRANS ("Dominant Ninth");
        case Type::minorMajorNinth:             return TRANS ("Minor Major Ninth");
        case Type::minorDominantNinth:          return TRANS ("Minor Dominant Ninth");
        case Type::augmentedMajorNinth:         return TRANS ("Augmented Major Ninth");
        case Type::augmentedDominantNinth:      return TRANS ("Augmented Dominant Ninth");
        case Type::halfDiminishedNinth:         return TRANS ("Half Diminished Ninth");
        case Type::halfDiminishedMinorNinth:    return TRANS ("Half Diminished Minor Ninth");
        case Type::diminishedNinth:             return TRANS ("Diminished Ninth");
        case Type::diminishedMinorNinth:        return TRANS ("Diminished Minor Ninth");
        case Type::custom:                      jassert (symbol.isNotEmpty()); return symbol;
        default: jassertfalse;                  return {};
    }
}

String Chord::getShortName() const
{
    switch (type)
    {
        case Type::majorTriad:                  return TRANS ("major");
        case Type::minorTriad:                  return TRANS ("minor");
        case Type::diminishedTriad:             return TRANS ("dim");
        case Type::augmentedTriad:              return TRANS ("aug");
        case Type::majorSixth:                  return TRANS ("major 6");
        case Type::minorSixth:                  return TRANS ("minor 6");
        case Type::dominantSeventh:             return TRANS ("dom 7");
        case Type::majorSeventh:                return TRANS ("major 7");
        case Type::minorSeventh:                return TRANS ("minor 7");
        case Type::augmentedSeventh:            return TRANS ("aug 7");
        case Type::diminishedSeventh:           return TRANS ("dim 7");
        case Type::halfDiminishedSeventh:       return TRANS ("half dim 7");
        case Type::minorMajorSeventh:           return TRANS ("min maj 7");
        case Type::power:                       return TRANS ("power");
        case Type::suspendedSecond:             return TRANS ("sus 2");
        case Type::suspendedFourth:             return TRANS ("sus 4");
        case Type::majorNinth:                  return TRANS ("major 9");
        case Type::dominantNinth:               return TRANS ("dom 9");
        case Type::minorMajorNinth:             return TRANS ("min maj 9");
        case Type::minorDominantNinth:          return TRANS ("min dom 9");
        case Type::augmentedMajorNinth:         return TRANS ("aug maj 9");
        case Type::augmentedDominantNinth:      return TRANS ("aug dom 9");
        case Type::halfDiminishedNinth:         return TRANS ("half dim 9");
        case Type::halfDiminishedMinorNinth:    return TRANS ("half dim min 9");
        case Type::diminishedNinth:             return TRANS ("dim 9");
        case Type::diminishedMinorNinth:        return TRANS ("dim min 9");
        case Type::custom:                      jassert (symbol.isNotEmpty()); return symbol;
        default: jassertfalse;                  return {};
    }
}

String Chord::getSymbol() const
{
    switch (type)
    {
        case Type::majorTriad:                  return "M";
        case Type::minorTriad:                  return "m";
        case Type::diminishedTriad:             return "o";
        case Type::augmentedTriad:              return "+";
        case Type::majorSixth:                  return "6";
        case Type::minorSixth:                  return "m6";
        case Type::dominantSeventh:              return "7";
        case Type::majorSeventh:                return "M7";
        case Type::minorSeventh:                return "m7";
        case Type::augmentedSeventh:            return "+7";
        case Type::diminishedSeventh:           return "o7";
        case Type::halfDiminishedSeventh:       return oslash + "7";
        case Type::minorMajorSeventh:           return "mM7";
        case Type::suspendedSecond:             return "sus2";
        case Type::suspendedFourth:             return "sus4";
        case Type::power:                       return "5";
        case Type::majorNinth:                  return "M9";
        case Type::dominantNinth:               return "9";
        case Type::minorMajorNinth:             return "mM9";
        case Type::minorDominantNinth:          return "m9";
        case Type::augmentedMajorNinth:         return "+M9";
        case Type::augmentedDominantNinth:      return "+9";
        case Type::halfDiminishedNinth:         return oslash + "9";
        case Type::halfDiminishedMinorNinth:    return oslash + flat + "9";
        case Type::diminishedNinth:             return "o9";
        case Type::diminishedMinorNinth:        return "o" + flat + "9";
        case Type::custom:                      jassert (symbol.isNotEmpty()); return symbol;
        default: jassertfalse;                  return {};
    }
}

Array<int> Chord::getSteps() const
{
    switch (type)
    {
        case Type::majorTriad:                  return { 0, 4, 7 };
        case Type::minorTriad:                  return { 0, 3, 7 };
        case Type::diminishedTriad:             return { 0, 3, 6 };
        case Type::augmentedTriad:              return { 0, 4, 8 };
        case Type::majorSixth:                  return { 0, 4, 7, 9 };
        case Type::minorSixth:                  return { 0, 3, 7, 9 };
        case Type::dominantSeventh:             return { 0, 4, 7, 10 };
        case Type::majorSeventh:                return { 0, 4, 7, 11 };
        case Type::minorSeventh:                return { 0, 3, 7, 10 };
        case Type::augmentedSeventh:            return { 0, 4, 8, 10 };
        case Type::diminishedSeventh:           return { 0, 3, 6, 9  };
        case Type::halfDiminishedSeventh:       return { 0, 3, 6, 10 };
        case Type::minorMajorSeventh:           return { 0, 3, 7, 11 };
        case Type::suspendedSecond:             return { 0, 2, 7 };
        case Type::suspendedFourth:             return { 0, 5, 7 };
        case Type::power:                       return { 0, 7 };
        case Type::majorNinth:                  return { 0, 4, 7, 11, 14 };
        case Type::dominantNinth:               return { 0, 4, 7, 10, 14 };
        case Type::minorMajorNinth:             return { 0, 3, 7, 11, 14 };
        case Type::minorDominantNinth:          return { 0, 3, 7, 10, 14 };
        case Type::augmentedMajorNinth:         return { 0, 4, 8, 11, 14 };
        case Type::augmentedDominantNinth:      return { 0, 4, 8, 10, 14 };
        case Type::halfDiminishedNinth:         return { 0, 3, 6, 10, 14 };
        case Type::halfDiminishedMinorNinth:    return { 0, 3, 6, 10, 13 };
        case Type::diminishedNinth:             return { 0, 3, 6, 9, 14 };
        case Type::diminishedMinorNinth:        return { 0, 3, 6, 9, 13 };
        case Type::custom:                      return steps;
        default:                                return {};
    }
}

Array<int> Chord::getSteps (int inversion) const
{
    auto res = getSteps();

    if (inversion > 0)
    {
        for (int i = 0; i < inversion; i++)
            res.add (res.removeAndReturn (0) + 12);
    }
    else if (inversion < 0)
    {
        for (int i = 0; i < -inversion; i++)
            res.insert (0, res.removeAndReturn (res.size() - 1) - 12);
    }

    return res;
}
