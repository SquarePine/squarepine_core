//==============================================================================
/** @returns the cube of its argument; n ^ 3. */
template<typename Type>
[[nodiscard]] constexpr Type cube (Type n) noexcept             { return square (n) * n; }

/** @returns the biquadrate of its argument; n ^ 4. */
template<typename Type>
[[nodiscard]] constexpr Type biquadrate (Type n) noexcept       { return square (n) * square (n); }

/** @returns the sursolid of its argument; n ^ 5. */
template<typename Type>
[[nodiscard]] constexpr Type sursolid (Type n) noexcept         { return biquadrate (n) * n; }

/** @returns the zenzicube of its argument; n ^ 6. */
template<typename Type>
[[nodiscard]] constexpr Type zenzicube (Type n) noexcept        { return cube (n) * cube (n); }

/** @returns the zenzicube of its argument; n ^ 7. */
template<typename Type>
[[nodiscard]] constexpr Type bissursolid (Type n) noexcept      { return zenzicube (n) * n; }

/** @returns the zenzizenzicubic of its argument; n ^ 12. */
template<typename Type>
[[nodiscard]] constexpr Type zenzizenzicubic (Type n) noexcept  { return zenzicube (n) * zenzicube (n); }
