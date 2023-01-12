/** Inherit from this to be able to uniquely identify your class instance using a juce::Uuid,
    as well as identifying your class instance with a standard juce::Identifier
    which is juce::ValueTree and generally juce::var compatible.

    @see juce::Uuid
*/
class Identifiable
{
public:
    /** */
    Identifiable() : identifier (String (Random::getSystemRandom().nextInt64())) { }

    /** */
    Identifiable (const Identifier& id) noexcept : identifier (id) { }

    /** */
    virtual ~Identifiable() noexcept = default;

    //==============================================================================
    /** */
    [[nodiscard]] const Identifier& getIdentifier() const noexcept { return identifier; }

    /** */
    [[nodiscard]] const Uuid& getUuid() const noexcept { return uuid; }

protected:
    //==============================================================================
    const Identifier identifier;
    const Uuid uuid;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Identifiable)
};
