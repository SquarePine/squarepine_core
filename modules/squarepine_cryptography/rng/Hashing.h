namespace std
{
    /** JUCE doesn't yet provide all possible std::hash overloads, so here's one for Identifier. */
    template<>
    struct hash<juce::Identifier>
    {
        /** */
        size_t operator() (const juce::Identifier& key) const noexcept
        {
            return std::hash<juce::String>() (key.toString());
        }
    };

    /** JUCE doesn't yet provide all possible std::hash overloads, so here's one for File. */
    template<>
    struct hash<juce::File>
    {
        /** */
        size_t operator() (const juce::File& key) const noexcept
        {
            return std::hash<juce::String>() (key.getFullPathName());
        }
    };

    /** JUCE doesn't yet provide all possible std::hash overloads, so here's one for MD5. */
    template<>
    struct hash<juce::MD5>
    {
        /** */
        size_t operator() (const juce::MD5& key) const
        {
            return std::hash<juce::String>() (key.toHexString());
        }
    };

    /** JUCE doesn't yet provide all possible std::hash overloads, so here's one for SHA256. */
    template<>
    struct hash<juce::SHA256>
    {
        /** */
        size_t operator() (const juce::SHA256& key) const
        {
            return std::hash<juce::String>() (key.toHexString());
        }
    };

    /** JUCE doesn't yet provide all possible std::hash overloads, so here's one for Whirlpool. */
    template<>
    struct hash<juce::Whirlpool>
    {
        /** */
        size_t operator() (const juce::Whirlpool& key) const
        {
            return std::hash<juce::String>() (key.toHexString());
        }
    };

    /** JUCE doesn't yet provide all possible std::hash overloads, so here's one for RSAKey. */
    template<>
    struct hash<juce::RSAKey>
    {
        /** */
        size_t operator() (const juce::RSAKey& key) const
        {
            return std::hash<juce::String>() (key.toString());
        }
    };
}
