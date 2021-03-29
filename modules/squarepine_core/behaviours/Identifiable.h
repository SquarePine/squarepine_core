/** */
class Identifiable
{
public:
    /** */
    Identifiable (const Identifier& id) noexcept : identifier (id) { }

    /** */
    virtual ~Identifiable() noexcept = default;

    //==============================================================================
    /** */
    squarepine_nodiscard const Identifier& getIdentifier() const noexcept { return identifier; }

    /** */
    squarepine_nodiscard const Uuid& getUuid() const noexcept { return uuid; }

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
