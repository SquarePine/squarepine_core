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
    JUCE_NODISCARD double getValue() const                  { return getDouble (value); }
    /** */
    JUCE_NODISCARD Value& getValueValue() noexcept          { return value; }

    /** */
    void setDefaultValue (double newDefaultValue) noexcept
    {
    }

    /** */
    JUCE_NODISCARD double getDefaultValue() const           { return getDouble (defaultValue); }
    /** */
    JUCE_NODISCARD Value& getDefaultValueValue() noexcept   { return defaultValue; }

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
    JUCE_NODISCARD TimelinePoint& getStart() noexcept       { return start; }
    /** */
    JUCE_NODISCARD TimelinePoint& getEnd() noexcept         { return end; }

    //==============================================================================
    /** */
    void setTime (double newTime) noexcept
    {
    }

    /** */
    JUCE_NODISCARD double getTime() const                   { return getDouble (time); }
    /** */
    JUCE_NODISCARD Value& getTimeValue() noexcept           { return time; }

    /** */
    void setLengthSeconds (double newLength) noexcept
    {
    }

    /** */
    JUCE_NODISCARD double getLengthSeconds() const          { return getDouble (length); }
    /** */
    JUCE_NODISCARD Value& getLengthValue() noexcept         { return length; }

    /** */
    void setFramerate (int newFramerate) noexcept
    {
    }

    /** */
    JUCE_NODISCARD double getFramerate() const              { return getInt (framerate); }
    /** */
    JUCE_NODISCARD Value& getFramerateValue() noexcept      { return framerate; }

    /** */
    void setPlaybackSpeed (double newSpeed) noexcept
    {
    }

    /** */
    JUCE_NODISCARD double getPlaybackSpeed() const          { return getDouble (speed); }
    /** */
    JUCE_NODISCARD Value& getPlaybackSpeedValue() noexcept  { return speed; }

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
