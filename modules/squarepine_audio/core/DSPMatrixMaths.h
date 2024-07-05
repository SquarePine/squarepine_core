/**
    arma::join_horiz
    arma::join_vert
*/

/**
    .submat()
    .i()
    .t()
    .shed_col
*/

namespace std
{
    /** */
    template<typename FloatType, class PredicateFunc>
    inline bool all_of (const juce::dsp::Matrix<FloatType>& mat, PredicateFunc func)
    {
        return std::all_of (std::cbegin (mat), std::cbegin (mat), func);
    }

    /** */
    template<typename FloatType>
    inline bool all_of (const juce::dsp::Matrix<FloatType>& mat)
    {
        return all_of (mat, [] (FloatType v) { return v != static_cast<FloatType> (0); });
    }

    /** */
    template<typename FloatType, class PredicateFunc>
    inline bool any_of (const juce::dsp::Matrix<FloatType>& mat, PredicateFunc func)
    {
        return std::any_of (std::cbegin (mat), std::cbegin (mat), func);
    }

    /** */
    template<typename FloatType>
    inline bool any_of (const juce::dsp::Matrix<FloatType>& mat)
    {
        return any_of (mat, [] (FloatType v) { return v != static_cast<FloatType> (0); });
    }

    /** */
    template<typename FloatType>
    inline juce::dsp::Matrix<FloatType> abs (const juce::dsp::Matrix<FloatType>& source)
    {
        juce::dsp::Matrix<FloatType> dest (source);
        for (auto& v : dest)
            v = std::abs (v);

        return dest;
    }
}

namespace sp
{
    /** */
    template<typename FloatType>
    inline juce::dsp::Matrix<FloatType> vectorise (const juce::dsp::Matrix<FloatType>& source)
    {
        return source;
    }

    /** */
    template<typename FloatType>
    inline juce::dsp::Matrix<FloatType> solve (const juce::dsp::Matrix<FloatType>& a,
                                               const juce::dsp::Matrix<FloatType>& b)
    {
        return a;
    }



    /** */
    template<typename FloatType>
    inline juce::dsp::Matrix<FloatType> joinHorizontal (const juce::dsp::Matrix<FloatType>& a,
                                                        const juce::dsp::Matrix<FloatType>& b)
    {
        return a;
    }

    /** */
    template<typename FloatType>
    inline juce::dsp::Matrix<FloatType> joinVertical (const juce::dsp::Matrix<FloatType>& a,
                                                      const juce::dsp::Matrix<FloatType>& b)
    {
        return a;
    }
}
