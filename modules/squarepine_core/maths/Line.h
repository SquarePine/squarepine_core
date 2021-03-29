/** Solves for the slope and intercept of a line. */
template<typename ValueType>
inline bool solveLine (ValueType x1, ValueType y1,
                       ValueType x2, ValueType y2,
                       ValueType& m, ValueType& b) noexcept
{
    if (x2 != x1)
    {
        m = (y2 - y1) / (x2 - x1);
        b = y2 - m * x2;
        return true;
    }

    m = b = static_cast<ValueType> (0);
    return false;
}

/** Solves for the slope and intercept of a line. */
template<typename ValueType>
inline bool solveLine (Line<ValueType> l, ValueType& m, ValueType& b) noexcept
{
    return solveLine (l.getStartX(), l.getEndX(),
                      l.getStartY(), l.getEndY(),
                      m, b);
}
