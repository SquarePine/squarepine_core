namespace
{
    inline DynamicObject::Ptr toDynamicObj (const ValueTree& root)
    {
        DynamicObject::Ptr result (new DynamicObject());

        for (int i = 0; i < root.getNumProperties(); ++i)
        {
            const auto name = root.getPropertyName (i);
            result->setProperty (name, root[name]);
        }

        for (const auto& child : root)
            if (auto dynObj = toDynamicObj (child))
                result->setProperty (child.getType(), dynObj.get());

        return result;
    }

    inline ValueTree toValueTree (DynamicObject* ptr, const Identifier& rootId)
    {
        if (ptr == nullptr)
            return {};

        ValueTree result (rootId);

        for (auto& p : ptr->getProperties())
        {
            if (auto* dynObj = p.value.getDynamicObject())
                result.appendChild (toValueTree (dynObj, p.name), nullptr);
            else
                result.setProperty (p.name, p.value, nullptr);
        }

        return result;
    }
}

ValueTree createValueTreeFromJSON (const var& json, const Identifier& rootId)
{
    return toValueTree (json.getDynamicObject(), rootId);
}

ValueTree createValueTreeFromJSON (const String& data, const Identifier& rootId)
{
    return createValueTreeFromJSON (JSON::parse (data), rootId);
}

String toJSONString (const ValueTree& tree)
{
    if (auto dynObj = toDynamicObj (tree))
    {
        MemoryOutputStream stream;
        dynObj->writeAsJSON (stream, 4, false, 3);
        return stream.getMemoryBlock().toString();
    }

    return {};
}
