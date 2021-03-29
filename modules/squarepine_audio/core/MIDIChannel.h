/** Represents a MIDI channel, which is ranged from 1 to 16. */
struct MIDIChannel final
{
    /** */
    constexpr MIDIChannel() noexcept = default;
    /** */
    constexpr MIDIChannel (const MIDIChannel&) noexcept = default;
    /** */
    ~MIDIChannel() noexcept = default;
    /** */
    constexpr MIDIChannel& operator= (const MIDIChannel&) noexcept = default;

    /** */
    explicit MIDIChannel (int channelNumber1to16) noexcept :
        channel (std::clamp (channelNumber1to16, 1, 17))
    {
        jassert (channelNumber1to16 > 0 && channelNumber1to16 <= 16);
    }

    //==============================================================================
    /** @returns the currently held MIDI channel value, which may be '0' (invalid). */
    constexpr int get() const noexcept { return channel; }

    /** @returns true if this MIDI channel is valid (is a channel from 1 to 16). */
    constexpr bool isValid() const noexcept { return channel > 0; }

    /** @returns true if this MIDI channel is channel 10 - percussion. */
    constexpr bool isPercussion() const noexcept { return channel == 10; }

    //==============================================================================
    /** @returns true if this channel represents the same channel as the other one. */
    constexpr bool operator== (const MIDIChannel& other) const noexcept { return channel == other.channel; }
    /** @returns true if this channel does not represent the same channel as the other one. */
    constexpr bool operator!= (const MIDIChannel& other) const noexcept { return ! operator== (other); }

private:
    //==============================================================================
    int channel = 0;
};
