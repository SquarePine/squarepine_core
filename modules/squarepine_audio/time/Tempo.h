/** Use an instance of this to track a tempo, in beats per minute.

    @see TimeSignature
*/
class Tempo final
{
public:
    //==============================================================================
    /** The default tempo in beats. */
    static constexpr double defaultTempo = 120.0;

    /** The minimum tempo in beats. */
    static constexpr double minimumTempo = 1.0;

    /** The maximum tempo in beats. */
    static constexpr double maximumTempo = 1000.0;

    //==============================================================================
    /** Default constructor, creating a tempo with a value of `defaultTempo`. */
    Tempo() noexcept = default;

    /** */
    explicit Tempo (double startingTempo) noexcept;

    /** Copy constructor. */
    Tempo (const Tempo&) noexcept = default;

    /** Move constructor. */
    Tempo (Tempo&&) noexcept = default;

    /** Destructor. */
    ~Tempo() noexcept = default;

    //==============================================================================
    /** */
    operator double() const noexcept { return value; }

    //==============================================================================
    /** @returns the first tempo message metadata found the
        provided reader, or the default tempo.
    */
    static Tempo fromReader (const AudioFormatReader&);

    /** @returns the first tempo message found in the MIDI file.

        @note Properly formatted MIDI files of type 1 should
        contain a tempo event on track 1.
    */
    static Tempo fromMIDIFile (const MidiFile&);

    //==============================================================================
    /** */
    Tempo& operator= (const Tempo&) noexcept = default;
    /** */
    Tempo& operator= (Tempo&&) noexcept = default;

    /** */
    bool operator== (const Tempo& other) const noexcept;
    /** */
    bool operator!= (const Tempo& other) const noexcept;
    /** */
    bool operator< (const Tempo& other) const noexcept;
    /** */
    bool operator<= (const Tempo& other) const noexcept;
    /** */
    bool operator> (const Tempo& other) const noexcept;
    /** */
    bool operator>= (const Tempo& other) const noexcept;

    //==============================================================================
    double value = defaultTempo; //< The tempo value, in beats per minute.
};
