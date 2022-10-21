namespace jsonhelpers
{
    const Identifier rootId = "root";
    const Identifier propertyId = "property";

    inline void setProperty (ValueTree& source, const Identifier& id, const var& v)
    {
        jassert (id.isValid() && ! v.isVoid());
        source.setProperty (id, v, nullptr);
    }

    inline void appendValueTree (ValueTree& parent, const Identifier& id, const var& v)
    {
        if (! parent.isValid() || id.isNull() || v.isVoid())
            return;

        if (auto* ar = v.getArray())
        {
            for (const auto& item : *ar)
            {
                ValueTree child (rootId);
                parent.appendChild (child, nullptr);
                appendValueTree (child, propertyId, item);
            }

            return;
        }

        if (auto* object = v.getDynamicObject())
        {
            ValueTree child (id);
            parent.appendChild (child, nullptr);

            for (const auto& prop : object->getProperties())
                appendValueTree (parent, prop.name, prop.value);

            return;
        }

        ValueTree child (id);
        parent.appendChild (child, nullptr);
        setProperty (child, Identifier (String ("property") + String (parent.getNumProperties())), v);
    }
}

ValueTree createValueTreeFromJSON (const var& json)
{
    using namespace jsonhelpers;

    if (json.isVoid())
        return {};

    ValueTree root (rootId);
    appendValueTree (root, rootId, json);
    return root;
}

ValueTree createValueTreeFromJSON (const String& data)
{
    return createValueTreeFromJSON (JSON::parse (data));
}

//==============================================================================
var createJSONFromXML (const XmlElement&)
{
    jassertfalse; // @todo
    return var::undefined();
}
