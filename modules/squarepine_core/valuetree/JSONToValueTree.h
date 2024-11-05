//==============================================================================
/** @returns a ValueTree created from a variant containing JSON. */
ValueTree createValueTreeFromJSON (const var& json, const Identifier& rootId);

/** @returns a ValueTree created from a String containing JSON. */
ValueTree createValueTreeFromJSON (const String& data, const Identifier& rootId);

/** @returns a JSON string from a ValueTree. */
String toJSONString (const ValueTree&);
