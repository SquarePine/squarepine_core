//==============================================================================
/** @returns */
ValueTree createValueTreeFromJSON (const var& json, const Identifier& rootId);

/** @returns */
ValueTree createValueTreeFromJSON (const String& data, const Identifier& rootId);

/** @returns */
String toJSONString (const ValueTree&);
