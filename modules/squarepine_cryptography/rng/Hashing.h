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

    //============================================================================
   #if 0 // TODO
    /** Here's an std::hash overload for SHA1. */
    template<>
    struct hash<sp::SHA1>
    {
        /** */
        size_t operator() (const sp::SHA1& key) const
        {
            return std::hash<juce::String>() (key.toHexString());
        }
    };
   #endif
}

//============================================================================
/** Creates a unique hash based on the contents of the provided source file.

    @param source   The source file to hash.
    @param hash     The destination hash, which will be empty if anything failed.

    @returns a non-empty string if the file could be hashed.
*/
template<typename HasherType = juce::SHA256>
[[nodiscard]] inline juce::String createUniqueFileHash (const juce::File& source)
{
    if (! source.existsAsFile())
    {
        jassertfalse;
        return {};
    }

    juce::FileInputStream fis (source);
    if (fis.failedToOpen())
    {
        jassertfalse;
        return {};
    }

    constexpr auto oneMiB = 1 << 20;
    if (fis.getTotalLength() < (oneMiB * 2))
        return HasherType (fis).toHexString();

    juce::BufferedInputStream bis (&fis, oneMiB, false);
    return HasherType (bis).toHexString();
}
