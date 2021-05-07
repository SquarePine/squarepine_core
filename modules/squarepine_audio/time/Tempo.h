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

    /** Attemps creating a tempo with the provided starting value.

        If this provided value is out of range, NaN, or Inf,
        the resulting tempo will be `defaultTempo`.
    */
    explicit Tempo (double startingTempo) noexcept;

    /** Copy constructor. */
    Tempo (const Tempo&) noexcept = default;

    /** Move constructor. */
    Tempo (Tempo&&) noexcept = default;

    /** Destructor. */
    ~Tempo() noexcept = default;

    //==============================================================================
    /** */
    constexpr operator double() const noexcept    { return value; }
    /** */
    constexpr double get() const noexcept         { return value; }

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
    /** Adds two tempos together */
    Tempo operator+ (const Tempo& other) const noexcept;

    /** Adds another tempo to this one. */
    Tempo& operator+= (const Tempo& other) noexcept;

    /** Subtracts one tempo from another. */
    Tempo operator- (const Tempo& other) const noexcept;

    /** Subtracts another value to this one. */
    Tempo& operator-= (const Tempo& other) noexcept;

    /** Multiplies two tempos together. */
    Tempo operator* (const Tempo& other) const noexcept;

    /** Multiplies another tempo to this one. */
    Tempo& operator*= (const Tempo& other) noexcept;

    /** Divides one tempo by another. */
    Tempo operator/ (const Tempo& other) const noexcept;

    /** Divides this tempo by another. */
    Tempo& operator/= (const Tempo& other) noexcept;

    /** @returns a tempo multiplied by a given scalar value. */
    Tempo operator* (double multiplier) const noexcept;

    /** @returns a tempo divided by a given scalar value. */
    Tempo operator/ (double divisor) const noexcept;

    /** Multiplies the point's coordinates by a scalar value. */
    Tempo& operator*= (double multiplier) noexcept;

    /** Divides the point's coordinates by a scalar value. */
    Tempo& operator/= (double divisor) noexcept;

private:
    //==============================================================================
    double value = defaultTempo; //< The tempo value, in beats per minute.

    //==============================================================================
    Tempo operator-() const = delete;

    static double snapValue (double t) noexcept
    {
        if (std::isnan (t) || std::isinf (t))
            return Tempo::defaultTempo;

        return std::clamp (t, Tempo::minimumTempo, Tempo::maximumTempo);
    }
};
