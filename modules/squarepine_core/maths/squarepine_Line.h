/** Solves for the slope and intercept of a line. */
template<typename Type>
inline bool solveLine (Type x1, Type y1,
                       Type x2, Type y2,
                       Type& m, Type& b) noexcept
{
    if (x2 != x1)
    {
        m = (y2 - y1) / (x2 - x1);
        b = y2 - m * x2;
        return true;
    }

    m = b = static_cast<Type> (0);
    return false;
}

/** Solves for the slope and intercept of a line. */
template<typename Type>
inline bool solveLine (Line<Type> l, Type& m, Type& b) noexcept
{
    return solveLine (l.getStartX(), l.getEndX(),
                      l.getStartY(), l.getEndY(),
                      m, b);
}
