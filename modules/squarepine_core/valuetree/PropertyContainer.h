/** A container of properties for a ValueTree.

    @see ValueTree
*/
class PropertyContainer : public ReferenceCountedObject
{
public:
    /** */
    explicit PropertyContainer (const Identifier& dataId) : data (dataId) { }

    /** */
    virtual ~PropertyContainer() = default;

    /** */
    ValueTree get() const noexcept { return data; }

    /** */
    Identifier getType() const noexcept { return data.getType(); }

    /** */
    bool hasType (const Identifier& typeName) const noexcept { return data.hasType (typeName); }

    /** */
    virtual void setProperty (const Identifier& id, const var& value, UndoManager* undoManager)
    {
        data.setProperty (id, value, undoManager);
    }

    /** */
    virtual var getProperty (const Identifier& id) const { return data.getProperty (id); }

    /** */
    Value getPropertyAsValue (const Identifier& id, UndoManager* undoManager, bool shouldUpdateSynchronously = false)
    {
        return data.getPropertyAsValue (id, undoManager, shouldUpdateSynchronously);
    }

    /** */
    bool getBool (const Identifier& id) const noexcept      { return getVarAsBool (data, id); }
    /** */
    int getInt (const Identifier& id) const noexcept        { return getVarAsInt (data, id); }
    /** */
    int64 getInt64 (const Identifier& id) const noexcept    { return getVarAsInt64 (data, id); }
    /** */
    float getFloat (const Identifier& id) const noexcept    { return getVarAsFloat (data, id); }
    /** */
    double getDouble (const Identifier& id) const noexcept  { return getVarAsDouble (data, id); }
    /** */
    String getString (const Identifier& id) const noexcept  { return getVarAsString (data, id); }

protected:
    ValueTree data;

private:
    PropertyContainer() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyContainer)
};
