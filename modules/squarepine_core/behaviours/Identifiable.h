/** Inherit from this to be able to uniquely identify your class instance (using a juce::Uuid),
    as well as identifying your class instance with a standard juce::Identifier
    which is juce::ValueTree and generally juce::var compatible.
*/
class Identifiable
{
public:
    /** */
    Identifiable (const Identifier& id) noexcept : identifier (id) { }

    /** */
    virtual ~Identifiable() noexcept = default;

    //==============================================================================
    /** */
    sp_nodiscard const Identifier& getIdentifier() const noexcept { return identifier; }

    /** */
    sp_nodiscard const Uuid& getUuid() const noexcept { return uuid; }

    //==============================================================================
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (uniqueIdentifier)

protected:
    //==============================================================================
    const Identifier identifier;
    const Uuid uuid;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Identifiable)
};
