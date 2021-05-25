//==============================================================================
/** */
#define CREATE_IDENTIFIER(name) \
    static const Identifier name##Id;

/** */
#define IMPLEMENT_IDENTIFIER(name, value) \
    const Identifier name##Id (value);

/** */
#define IMPLEMENT_IDENTIFIER_FROM_CLASS(className, name, value) \
    const Identifier className::name##Id = value;

/** */
#define CREATE_INLINE_IDENTIFIER(name) \
    static const Identifier name##Id (JUCE_STRINGIFY (name));

#if DOXYGEN || JUCE_CXX17_IS_AVAILABLE
    /** */
    #define CREATE_INLINE_CLASS_IDENTIFIER(name) \
        inline static const Identifier name##Id = JUCE_STRINGIFY (name);
#endif

//==============================================================================
/** Appends a child tree to the parent tree, and returns the child.

    Note that this is functionally equivalent to calling ValueTree::append().
*/
inline ValueTree append (ValueTree& parent, const ValueTree& newChild, UndoManager* undoManager = nullptr)
{
    parent.appendChild (newChild, undoManager);
    return newChild;
}

/** Creates a child tree and appends it to the parent tree, and returns the newly created child. */
inline ValueTree append (ValueTree& parent, const Identifier& id, UndoManager* undoManager = nullptr)
{
    return append (parent, ValueTree (id), undoManager);
}

//==============================================================================
/** */
inline bool getVarAsBool (const ValueTree& vt, const Identifier& id)                { return static_cast<bool> (vt.getProperty (id)); }
/** */
inline int getVarAsInt (const ValueTree& vt, const Identifier& id)                  { return static_cast<int> (vt.getProperty (id)); }
/** */
inline int64 getVarAsInt64 (const ValueTree& vt, const Identifier& id)              { return static_cast<int64> (vt.getProperty (id)); }
/** */
inline double getVarAsDouble (const ValueTree& vt, const Identifier& id)            { return static_cast<double> (vt.getProperty (id)); }
/** */
inline float getVarAsFloat (const ValueTree& vt, const Identifier& id)              { return static_cast<float> (getVarAsDouble (vt, id)); }
/** */
inline String getVarAsString (const ValueTree& vt, const Identifier& id)            { return vt.getProperty (id).toString(); }
/** */
inline Time getVarAsISO8601 (const ValueTree& vt, const Identifier& id)             { return Time::fromISO8601 (getVarAsString (vt, id)); }
/** */
inline Identifier getVarAsIdentifier (const ValueTree& vt, const Identifier& id)    { return static_cast<Identifier> (vt.getProperty (id)); }

//==============================================================================
/** Recursively finds an ancestor of a given type, if none is found a null ValueTree is returned. */
inline ValueTree getValueTreeParentOfType (const ValueTree& vt, const Identifier& type)
{
    auto parent = vt.getParent();
    if (parent.hasType (type) || ! parent.isValid())
        return parent;

    return getValueTreeParentOfType (parent, type);
}

inline void setPropertyIfMissing (ValueTree& v, const Identifier& id,
                                  const var& value, UndoManager* um)
{
    if (! v.hasProperty (id))
        v.setProperty (id, value, um);
}

/** @returns true if the value was set. */
template<typename Type>
inline bool setIfDifferent (Type& val, const Type& newVal) noexcept
{
    if (val == newVal)
        return false;

    val = newVal;
    return true;
}

/** @returns true if the cached value was set. */
template<typename Type>
inline bool setIfDifferent (CachedValue<Type>& val, const Type& newVal) noexcept
{
    if (val == newVal)
        return false;

    val = newVal;
    return true;
}

/** @returns true if the needle is found in the haystack. */
template<typename Type>
inline bool matchesAnyOf (const Type& needle, const std::initializer_list<Type>& haystack)
{
    for (auto&& h : haystack)
        if (h == needle)
            return true;

    return false;
}

/** Calls a function object on each item in an array. */
template<typename Type, typename UnaryFunction>
inline void forEachItem (const Array<Type*>& items, const UnaryFunction& fn)
{
    for (auto* f : items)
        fn (f);
}

//==============================================================================
/** Easy way to get one callback for all value tree change events. */
class ValueTreeAllEventListener : public ValueTree::Listener
{
public:
    /** */
    virtual void valueTreeChanged() = 0;

    /** @internal */
    void valueTreePropertyChanged (ValueTree&, const Identifier&) override  { valueTreeChanged(); }
    /** @internal */
    void valueTreeChildAdded (ValueTree&, ValueTree&) override              { valueTreeChanged(); }
    /** @internal */
    void valueTreeChildRemoved (ValueTree&, ValueTree&, int) override       { valueTreeChanged(); }
    /** @internal */
    void valueTreeChildOrderChanged (ValueTree&, int, int) override         { valueTreeChanged(); }
    /** @internal */
    void valueTreeParentChanged (ValueTree&) override                       { valueTreeChanged(); }
    /** @internal */
    void valueTreeRedirected (ValueTree&) override                          { valueTreeChanged(); }
};

//==============================================================================
/** */
template<typename Type>
struct ValueTreeComparator
{
public:
    /** */
    ValueTreeComparator (const Identifier& attributeToSort_, bool forwards)
        : attributeToSort (attributeToSort_), direction (forwards ? 1 : -1)
    {}

    /** */
    int compareElements (const ValueTree& first, const ValueTree& second) const
    {
        const auto result = (Type (first[attributeToSort]) > Type (second[attributeToSort])) ? 1 : -1;
        return direction * result;
    }

private:
    const Identifier attributeToSort;
    const int direction;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeComparator)
};

/** */
template<>
inline int ValueTreeComparator<String>::compareElements (const ValueTree& first, const ValueTree& second) const
{
    return direction * first[attributeToSort].toString().compareNatural (second[attributeToSort].toString());
}

//==============================================================================
/** Stores a collection of Properties and their new values, which are all set at once. */
class PropertyListUpdater final
{
public:
    /** */
    PropertyListUpdater (const ValueTree& vt, UndoManager* um) noexcept :
        source (vt),
        undoManager (um)
    {
    }

    /** */
    void add (const Identifier& property, double value)
    {
        propertiesToSet.add ({ property, value });
    }

    /** */
    void applyUpdates()
    {
        if (undoManager != nullptr)
            undoManager->beginNewTransaction();

        for (const auto& propertyToSet : propertiesToSet)
            source.setProperty (propertyToSet.property, propertyToSet.value, undoManager);
    }

private:
    /** */
    struct PropertiesToSet
    {
        Identifier property;
        double value = 0.0;
    };

    Array<PropertiesToSet> propertiesToSet;
    ValueTree source;
    UndoManager* undoManager = nullptr;

    PropertyListUpdater() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyListUpdater)
};

//==============================================================================
/** */
template<typename ValueType>
inline void initialiseValue (ValueTree& state, ValueType& v, const Identifier& id, ValueType defaultValue = ValueType())
{
    v = defaultValue;
    state.setProperty (id, VariantConverter<ValueType>::toVar (defaultValue), nullptr);
}

/** */
template<typename ValueType>
inline void initialiseCachedValue (ValueTree& state, CachedValue<ValueType>& cv, const Identifier& id, ValueType defaultValue = ValueType())
{
    cv.referTo (state, id, nullptr, defaultValue);
    state.setProperty (id, VariantConverter<ValueType>::toVar (defaultValue), nullptr);
}
