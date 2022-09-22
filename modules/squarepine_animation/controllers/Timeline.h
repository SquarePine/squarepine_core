/**
*/
class TimelinePoint final : public Identifiable
{
public:
    /** */
    TimelinePoint() = default;

    /** */
    TimelinePoint (const Identifier& id) noexcept :
        Identifiable (id)
    {
    }

    /** */
    ~TimelinePoint() override
    {
        masterReference.clear();
    }

    //==============================================================================
    /** */
    void setValue (double newValue) noexcept
    {
    }

    /** */
    [[nodiscard]] double getValue() const                   { return getDouble (value); }
    /** */
    [[nodiscard]] Value& getValueValue() noexcept           { return value; }

    /** */
    void setDefaultValue (double newDefaultValue) noexcept
    {
    }

    /** */
    [[nodiscard]] double getDefaultValue() const            { return getDouble (defaultValue); }
    /** */
    [[nodiscard]] Value& getDefaultValueValue() noexcept    { return defaultValue; }

private:
    //==============================================================================
    Value value { 0.0 },
          defaultValue { 0.0 };

    //==============================================================================
    constexpr double clampValue (double v)
    {
        return std::clamp (v, 0.0, 1.001);
    }

    static double getInt (const Value& v)
    {
        return static_cast<int> (v.getValue());
    }

    static double getDouble (const Value& v)
    {
        return static_cast<double> (v.getValue());
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimelinePoint)
    JUCE_DECLARE_WEAK_REFERENCEABLE (TimelinePoint)
};

//==============================================================================
/**
*/
class Timeline final : public Identifiable
{
public:
    /** */
    Timeline() = default;

    /** */
    Timeline (const Identifier& id) noexcept :
        Identifiable (id)
    {
    }

    /** */
    ~Timeline() override
    {
        masterReference.clear();
    }

    //==============================================================================
    /** */
    [[nodiscard]] TimelinePoint& getStart() noexcept        { return start; }
    /** */
    [[nodiscard]] TimelinePoint& getEnd() noexcept          { return end; }

    //==============================================================================
    /** */
    void setTime (double newTime) noexcept
    {
    }

    /** */
    [[nodiscard]] double getTime() const                    { return getDouble (time); }
    /** */
    [[nodiscard]] Value& getTimeValue() noexcept            { return time; }

    /** */
    void setLengthSeconds (double newLength) noexcept
    {
    }

    /** */
    [[nodiscard]] double getLengthSeconds() const           { return getDouble (length); }
    /** */
    [[nodiscard]] Value& getLengthValue() noexcept          { return length; }

    /** */
    void setFramerate (int newFramerate) noexcept
    {
    }

    /** */
    [[nodiscard]] double getFramerate() const               { return getInt (framerate); }
    /** */
    [[nodiscard]] Value& getFramerateValue() noexcept       { return framerate; }

    /** */
    void setPlaybackSpeed (double newSpeed) noexcept
    {
    }

    /** */
    [[nodiscard]] double getPlaybackSpeed() const           { return getDouble (speed); }
    /** */
    [[nodiscard]] Value& getPlaybackSpeedValue() noexcept   { return speed; }

    //==============================================================================
    /** */
    enum class Behaviour
    {
        singleShot,
        flipFlop,
        reverse
    };

private:
    //==============================================================================
    const double timeInterval = 0.0001;
    Value time { 0.0 },
          length { 1.0 },
          speed { 1.0 },
          framerate { var ((int) 0) };
    TimelinePoint start, end;
    // std::function<double () const> generator;
    OffloadedTimer timer;

    //==============================================================================
    static double getInt (const Value& v)
    {
        return static_cast<int> (v.getValue());
    }

    static double getDouble (const Value& v)
    {
        return static_cast<double> (v.getValue());
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Timeline)
    JUCE_DECLARE_WEAK_REFERENCEABLE (Timeline)
};
