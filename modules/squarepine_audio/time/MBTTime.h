/** Object used to take denote time in measures, beats and ticks */
class MBTTime final : public TimeFormat
{
public:
    //==============================================================================
    /** */
    static constexpr double defaultTicksResolution = 960.0;

    //==============================================================================
    MBTTime (int measure = 0, int beat = 0, int tick = 0);
    MBTTime (double ticks, int ppq, const Array<TimeSignature>& timeSigs, double songLength);
    MBTTime (const MBTTime& other);
    ~MBTTime() override;

    //==============================================================================
    double toTicks (const Array<TimeSignature>& timeSigs, int ppq);

    //==============================================================================
    static void ticksToMBTTime (MBTTime& out,
                                double ticks,
                                int ppq,
                                const Array<TimeSignature>& timeSigs,
                                double songLengthMIDITicks);

    //==============================================================================
    MBTTime& operator= (const MBTTime& other);
    bool operator== (const MBTTime& other) const;
    bool operator!= (const MBTTime& other) const;

    //==============================================================================
    /** @return String that will appear in the format "mmm:bbb:ttt" */
    String toString() const override;
    double toSeconds() const override;

    //==============================================================================
    int measure, beat, tick;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR (MBTTime)
};
