//==============================================================================
/** */
ValueTree createValueTreeFromJSON (const var& json);

/** */
ValueTree createValueTreeFromJSON (const String& data);

//==============================================================================
/** */
var createJSONFromXML (const XmlElement& xml);

/** */
var createJSONFromValueTree (const ValueTree& tree, const Identifier& rootArrayName);
