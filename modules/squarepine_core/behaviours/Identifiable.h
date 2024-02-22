/** Inherit from this to be able to uniquely identify your class instance using a juce::Uuid,
    as well as identifying your class instance with a standard juce::Identifier
    which is juce::ValueTree and generally juce::var compatible.

    @warning The provided Identifier must be JSON and XML compatible.

    @see juce::Uuid, juce::Identifier
*/
class Identifiable
{
public:
    /** */
    Identifiable (const Identifier& id) noexcept :
        identifier (id)
    {
        // You've done something rather silly if you hit this.
        jassert (Identifier::isValidIdentifier (id.toString()));
    }

    /** */
    virtual ~Identifiable() noexcept = default;

    //==============================================================================
    /** @returns the identifier for this object type. */
    [[nodiscard]] const Identifier& getIdentifier() const noexcept { return identifier; }

    /** @returns true if the Identifiable has the provided type.
        The comparison is case-sensitive.
        @see getIdentifier
    */
    [[nodiscard]] bool hasType (const Identifier& typeName) const noexcept { return identifier == typeName; }

    /** @returns the UUID for this object instance. */
    [[nodiscard]] const Uuid& getUuid() const noexcept { return uuid; }

protected:
    //==============================================================================
    const Identifier identifier;
    const Uuid uuid;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Identifiable)
};
