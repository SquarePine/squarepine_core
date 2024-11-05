/** Represents a MIDI channel, which is ranged from 1 to 16. */
struct MIDIChannel final
{
    /** Creates an invalid MIDI channel (with value 0). */
    constexpr MIDIChannel() noexcept = default;
    /** Creats a copy of a MIDI channel. */
    constexpr MIDIChannel (const MIDIChannel&) noexcept = default;
    /** Destructor. */
    ~MIDIChannel() noexcept = default;
    /** Copies another MIDI channel into this one. */
    constexpr MIDIChannel& operator= (const MIDIChannel&) noexcept = default;

    /** Creates a MIDI channel.

        The value will be appropriately clamped to 1 - 16 (within the valid MIDI channel range).
    */
    MIDIChannel (int channelNumber1to16) noexcept :
        channel (std::clamp (channelNumber1to16, 1, 16))
    {
        // Not sure what trick you're trying to pull...!
        jassert (isValueBetween (channelNumber1to16, 1, 16));
    }

    //==============================================================================
    /** @returns the currently held MIDI channel value, which may be '0' (invalid). */
    [[nodiscard]] constexpr int get() const noexcept                { return channel; }

    /** @returns true if this MIDI channel is valid (is a channel from 1 to 16). */
    [[nodiscard]] constexpr bool isValid() const noexcept           { return channel >= 1 && channel <= 16; }

    /** @returns true if this MIDI channel is channel 10. */
    [[nodiscard]] constexpr bool isPercussion() const noexcept      { return channel == 10; }

    //==============================================================================
    /** @returns true if this channel represents the same channel as the other one. */
    constexpr bool operator== (MIDIChannel other) const noexcept    { return channel == other.channel; }
    /** @returns true if this channel does not represent the same channel as the other one. */
    constexpr bool operator!= (MIDIChannel other) const noexcept    { return ! operator== (other); }

    /** */
    constexpr bool operator< (MIDIChannel other) const noexcept     { return channel < other.channel; }
    /** */
    constexpr bool operator<= (MIDIChannel other) const noexcept    { return channel <= other.channel; }
    /** */
    constexpr bool operator> (MIDIChannel other) const noexcept     { return channel > other.channel; }
    /** */
    constexpr bool operator>= (MIDIChannel other) const noexcept    { return channel >= other.channel; }

private:
    //==============================================================================
    int channel = 0;

    /** You can't negate a MIDI channel... */
    MIDIChannel operator-() const noexcept = delete;
};
