Scale::Scale (Type type_) :
    type (type_)
{
    switch (type)
    {
        case Type::major:
        case Type::ionian:
            steps       = { StepType::whole, StepType::whole, StepType::half, StepType::whole, StepType::whole, StepType::whole, StepType::half };
            triads      = generateTriads (0);
            sixths      = generateSixths (0);
            sevenths    = generateSevenths (0);
        break;

        case Type::dorian:
            steps       = { StepType::whole, StepType::half, StepType::whole, StepType::whole, StepType::whole, StepType::half, StepType::whole };
            triads      = generateTriads (1);
            sixths      = generateSixths (1);
            sevenths    = generateSevenths (1);
        break;

        case Type::phrygian:
            steps       = { StepType::half, StepType::whole, StepType::whole, StepType::whole, StepType::half, StepType::whole, StepType::whole };
            triads      = generateTriads (2);
            sixths      = generateSixths (2);
            sevenths    = generateSevenths (2);
        break;

        case Type::lydian:
            steps       = { StepType::whole, StepType::whole, StepType::whole, StepType::half, StepType::whole, StepType::whole, StepType::half };
            triads      = generateTriads (3);
            sixths      = generateSixths (3);
            sevenths    = generateSevenths (3);
        break;

        case Type::mixolydian:
            steps       = { StepType::whole, StepType::whole, StepType::half, StepType::whole, StepType::whole, StepType::half, StepType::whole };
            triads      = generateTriads (4);
            sixths      = generateSixths (4);
            sevenths    = generateSevenths (4);
        break;

        case Type::minor:
        case Type::aeolian:
            steps       = { StepType::whole, StepType::half, StepType::whole, StepType::whole, StepType::half, StepType::whole, StepType::whole };
            triads      = generateTriads (5);
            sixths      = generateSixths (5);
            sevenths    = generateSevenths (5);
        break;

        case Type::locrian:
            steps       = { StepType::half, StepType::whole, StepType::whole, StepType::half, StepType::whole, StepType::whole, StepType::whole };
            triads      = generateTriads (6);
            sixths      = generateSixths (6);
            sevenths    = generateSevenths (6);
        break;

        case Type::melodicMinor:
            steps       = { StepType::whole, StepType::half, StepType::whole, StepType::whole, StepType::whole, StepType::whole, StepType::half };
            triads      = { Chord::Type::minorTriad, Chord::Type::minorTriad, Chord::Type::augmentedTriad, Chord::Type::majorTriad, Chord::Type::majorTriad, Chord::Type::diminishedTriad, Chord::Type::diminishedTriad };
            sixths      = { Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid };
            sevenths    = { Chord::Type::minorMajorSeventh, Chord::Type::minorSeventh, Chord::Type::augmentedSeventh, Chord::Type::dominantSeventh, Chord::Type::dominantSeventh, Chord::Type::halfDiminishedSeventh, Chord::Type::halfDiminishedSeventh };
        break;

        case Type::harmonicMinor:
            steps       = { StepType::whole, StepType::half, StepType::whole, StepType::whole, StepType::half, StepType::wholeHalf, StepType::half };
            triads      = { Chord::Type::minorTriad, Chord::Type::diminishedTriad, Chord::Type::augmentedTriad, Chord::Type::minorTriad, Chord::Type::majorTriad, Chord::Type::majorTriad, Chord::Type::diminishedTriad };
            sixths      = { Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid, Chord::Type::invalid };
            sevenths    = { Chord::Type::minorMajorSeventh, Chord::Type::halfDiminishedSeventh, Chord::Type::augmentedSeventh, Chord::Type::minorSeventh, Chord::Type::dominantSeventh, Chord::Type::majorSeventh, Chord::Type::diminishedSeventh };
        break;

        default:
        break;
    }
}

Array<Scale::Type> Scale::getAllScaleTypes()
{
    Array<Type> res;

    for (int i = (int) Type::major; i <= (int) Type::harmonicMinor; ++i)
        res.add ((Type) i);

    return res;
}

StringArray Scale::getScaleStrings()
{
    StringArray res;

    for (int i = (int) Type::major; i <= (int) Type::harmonicMinor; ++i)
        res.add (getNameForType ((Type) i));

    return res;
}

Scale::Type Scale::getTypeFromName (const String& name)
{
    for (int i = (int) Type::major; i <= (int) Type::harmonicMinor; ++i)
        if (name == getNameForType ((Type) i))
            return (Type) i;

    jassertfalse;
    return Type::major;
}

String Scale::getName() const
{
    return getNameForType (type);
}

String Scale::getShortName() const
{
    return getShortNameForType (type);
}

String Scale::getNameForType (Type type)
{
    switch (type)
    {
        case Type::ionian:          return TRANS ("Ionian");
        case Type::dorian:          return TRANS ("Dorian");
        case Type::phrygian:        return TRANS ("Phrygian");
        case Type::lydian:          return TRANS ("Lydian");
        case Type::mixolydian:      return TRANS ("Mixolydian");
        case Type::aeolian:         return TRANS ("Aeolian");
        case Type::locrian:         return TRANS ("Locrian");
        case Type::major:           return TRANS ("Major");
        case Type::minor:           return TRANS ("Minor");
        case Type::melodicMinor:    return TRANS ("Melodic Minor");
        case Type::harmonicMinor:   return TRANS ("Harmonic Minor");
        default: jassertfalse;      return {};
    }
}

String Scale::getShortNameForType (Type type)
{
    switch (type)
    {
        case Type::ionian:          return TRANS ("Ion");
        case Type::dorian:          return TRANS ("Dor");
        case Type::phrygian:        return TRANS ("Phr");
        case Type::lydian:          return TRANS ("Lyd");
        case Type::mixolydian:      return TRANS ("Mix");
        case Type::aeolian:         return TRANS ("Aeo");
        case Type::locrian:         return TRANS ("Loc");
        case Type::major:           return TRANS ("Maj");
        case Type::minor:           return TRANS ("Min");
        case Type::melodicMinor:    return TRANS ("Mel");
        case Type::harmonicMinor:   return TRANS ("Har");
        default: jassertfalse;      return {};
    }
}

Array<Chord> Scale::generateTriads (int offset) const
{
    constexpr std::array<Chord::Type, 7> base =
    {
        Chord::Type::majorTriad, Chord::Type::minorTriad, Chord::Type::minorTriad,
        Chord::Type::majorTriad, Chord::Type::majorTriad, Chord::Type::minorTriad,
        Chord::Type::diminishedTriad
    };

    Array<Chord> res;

    constexpr auto numItems = base.size();
    for (size_t i = 0; i < numItems; ++i)
        res.add (base[(i + (size_t) offset) % numItems]);

    return res;
}

Array<Chord> Scale::generateSixths (int offset) const
{
    constexpr std::array<Chord::Type, 7> base =
    {
        Chord::Type::majorSixth, Chord::Type::minorSixth, Chord::Type::invalid,
        Chord::Type::majorSixth, Chord::Type::majorSixth, Chord::Type::invalid,
        Chord::Type::invalid
    };

    Array<Chord> res;

    constexpr auto numItems = base.size();
    for (size_t i = 0; i < numItems; ++i)
        res.add (base[(i + (size_t) offset) % numItems]);

    return res;
}

Array<Chord> Scale::generateSevenths (int offset) const
{
    constexpr std::array<Chord::Type, 7> base =
    {
        Chord::Type::majorSeventh, Chord::Type::minorSeventh, Chord::Type::minorSeventh,
        Chord::Type::majorSeventh, Chord::Type::dominantSeventh, Chord::Type::minorSeventh,
        Chord::Type::halfDiminishedSeventh
    };

    Array<Chord> res;

    constexpr auto numItems = base.size();
    for (size_t i = 0; i < numItems; ++i)
        res.add (base[(i + (size_t) offset) % numItems]);

    return res;
}

Array<int> Scale::getSteps (int octaves) const
{
    Array<int> res { 0 };
    int step = 0;

    for (int i = 0; i < steps.size() - 1; ++i)
    {
        int inc = 0;
        switch (steps[i])
        {
            case StepType::whole:       inc = 2; break;
            case StepType::half:        inc = 1; break;
            case StepType::wholeHalf:   inc = 3; break;
            default:                    jassertfalse; break;
        }

        step += inc;
        res.add (step);
    }

    if (octaves > 1)
    {
        const auto itemsInScale = res.size();
        for (int o = 1; o < octaves; ++o)
            for (int i = 0; i < itemsInScale; ++i)
                res.add (res[i] + o * 12);
    }

    return res;
}

StringArray Scale::getIntervalNames()
{
    return { "i", "ii", "iii", "iv", "v", "vi", "vii" };
}

String Scale::getIntervalName (IntervalType interval) const
{
    const auto name = getIntervalNames()[(int) interval];

    switch (triads[(int) interval].getType())
    {
        case Chord::Type::majorTriad:        return name.toUpperCase();
        case Chord::Type::augmentedTriad:    return name.toUpperCase() + "+";
        case Chord::Type::diminishedTriad:   return name + String::charToString (176);
        case Chord::Type::minorTriad:        break;
        default: jassertfalse; break;
    }

    return name;
}
